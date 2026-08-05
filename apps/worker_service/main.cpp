#include "elit21/aliexpress/AliExpressClient.h"
#include "elit21/catalog/CatalogManager.h"
#include "elit21/compliance/CanadaCompliance.h"
#include "elit21/config/Config.h"
#include "elit21/core/Result.h"
#include "elit21/core/Types.h"
#include "elit21/fulfillment/FulfillmentManager.h"
#include "elit21/inventory/InventoryManager.h"
#include "elit21/json/Json.h"
#include "elit21/logging/Logger.h"
#include "elit21/net/HttpClient.h"
#include "elit21/orders/OrderManager.h"
#include "elit21/pricing/PricingEngine.h"
#include "elit21/reports/ReportGenerator.h"
#include "elit21/risk/RiskEngine.h"
#include "elit21/security/Crypto.h"
#include "elit21/shopify/ShopifyClient.h"
#include "elit21/shopify/ShopifyPrivacyService.h"
#include "elit21/shopify/ShopifyReconciliationCoordinator.h"
#include "elit21/shopify/ShopifyWebhookRegistry.h"
#include "elit21/shopify/ShopifyWebhook.h"
#include "elit21/sourcing/SourcingEngine.h"
#include "elit21/storage/Database.h"

#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <filesystem>
#include <iostream>
#include <string>

namespace {

struct WorkerRuntime {
    explicit WorkerRuntime(elit21::Config value)
        : config(std::move(value)),
          logger(config.app.log_file),
          compliance(elit21::CanadaCompliance::loadKeywords("resources/prohibited_keywords.txt")),
          risk(config.risk),
          pricing(config.pricing),
          shopify(config.shopify, http),
          aliexpress(config.aliexpress, http),
          sourcing(config.sourcing, compliance, risk),
          catalog(aliexpress, shopify, pricing, sourcing, database, logger, config, counters),
          inventory(aliexpress, shopify, pricing, database, logger, config, counters),
          orders(shopify, aliexpress, compliance, risk, database, logger, config, counters),
          fulfillment(aliexpress, shopify, database, logger, counters),
          privacy(database, config.app.data_dir),
          webhook(config.shopify) {}

    elit21::Result<void> initialize() {
        std::filesystem::create_directories(config.app.data_dir);
        if (const auto parent = std::filesystem::path(config.app.log_file).parent_path(); !parent.empty()) {
            std::filesystem::create_directories(parent);
        }
        if (const auto parent = std::filesystem::path(config.app.database).parent_path(); !parent.empty()) {
            std::filesystem::create_directories(parent);
        }
        http.setTimeoutSeconds(config.network.request_timeout_seconds);
        http.setConnectTimeoutSeconds(config.network.connect_timeout_seconds);
        http.setMaximumResponseBytes(
            static_cast<std::size_t>(config.network.maximum_response_megabytes) * 1024U * 1024U);
        http.setTlsVerification(config.network.verify_tls);
        http.setCaBundle(config.network.ca_bundle);
        http.setProxy(config.network.proxy_url);

        auto opened = database.open(config.app.database);
        if (!opened) return opened;
        auto migrated = database.migrateDirectory(config.migrations_dir);
        if (!migrated) return migrated;
        auto recovered = database.recoverStaleTasks(900);
        if (!recovered) return elit21::Result<void>::failure(recovered.error());
        if (recovered.value() > 0) {
            logger.warning("worker", "Tâches bloquées récupérées: " + std::to_string(recovered.value()));
        }
        return elit21::Result<void>::success();
    }

    bool shopifyReady() const {
        return !config.shopify.shop.empty() && config.shopify.shop != "your-shop" &&
               !config.shopify.access_token.empty();
    }

    bool aliexpressReady() const {
        return !config.aliexpress.app_key.empty() && !config.aliexpress.app_secret.empty() &&
               !config.aliexpress.access_token.empty();
    }

    elit21::Result<void> execute(const elit21::TaskRecord& task, const elit21::Json& payload) {
        const auto before_errors = counters.errors;
        const bool reconciliation_task = payload.getBool("reconciliation_task", false);
        if (task.kind == "shopify_order_paid") {
            const auto webhook_id = payload.getString("webhook_id");
            const auto raw = payload.getString("payload");
            if (webhook_id.empty() || raw.empty()) {
                return elit21::Result<void>::failure("Task shopify_order_paid missing webhook_id or payload");
            }
            auto processing = database.markWebhookProcessing("shopify", webhook_id);
            if (!processing) return processing;
            auto order = webhook.parseOrderPaid(raw);
            if (!order) return elit21::Result<void>::failure(order.error());
            auto ingested = orders.ingest(order.take());
            if (!ingested) return ingested;
            return database.markWebhookProcessed("shopify", webhook_id);
        }
        if (task.kind == "shopify_order_cancelled") {
            auto raw = elit21::Json::parse(payload.getString("payload"));
            if (!raw) return elit21::Result<void>::failure("Invalid cancelled-order payload: " + raw.error());
            const auto order_id = raw.value().getScalarString("id");
            if (order_id.empty()) return elit21::Result<void>::failure("Cancelled order id is missing");
            auto current = database.ordersByShopifyIds({order_id});
            if (!current) return elit21::Result<void>::failure(current.error());
            if (!current.value().empty()) {
                const auto status = current.value().front().status;
                auto updated = database.transitionOrderStatus(order_id, {status}, "cancelled",
                                                              "shopify_order_cancelled", raw.value().dump());
                if (!updated) return elit21::Result<void>::failure(updated.error());
            }
            return database.audit("INFO", "shopify", "Order cancelled by Shopify", raw.value().dump(), order_id);
        }
        if (task.kind == "shopify_refund_created") {
            auto raw = elit21::Json::parse(payload.getString("payload"));
            if (!raw) return elit21::Result<void>::failure("Invalid refund payload: " + raw.error());
            const auto order_id = raw.value().getScalarString("order_id");
            if (!order_id.empty()) {
                auto current = database.ordersByShopifyIds({order_id});
                if (!current) return elit21::Result<void>::failure(current.error());
                if (!current.value().empty()) {
                    const auto status = current.value().front().status;
                    auto updated = database.transitionOrderStatus(order_id, {status}, "refund_received",
                                                                  "shopify_refund_created", raw.value().dump());
                    if (!updated) return elit21::Result<void>::failure(updated.error());
                }
            }
            return database.audit("INFO", "shopify", "Refund webhook received", raw.value().dump(), order_id);
        }
        if (task.kind == "shopify_product_updated" || task.kind == "shopify_product_deleted") {
            auto raw = elit21::Json::parse(payload.getString("payload"));
            if (!raw) return elit21::Result<void>::failure("Invalid product payload: " + raw.error());
            elit21::Json follow_up = elit21::Json::object();
            follow_up.set("reason", task.kind);
            follow_up.set("shopify_product_id", raw.value().getScalarString("id"));
            follow_up.set("source_webhook_id", payload.getString("webhook_id"));
            auto queued = database.enqueueUniqueTask(
                "catalog_sync", follow_up.dump(), 30, {},
                "catalog-sync:" + raw.value().getScalarString("id"));
            if (!queued) return elit21::Result<void>::failure(queued.error());
            return database.audit("INFO", "shopify", "Product reconciliation queued", follow_up.dump());
        }
        if (task.kind == "shopify_inventory_updated") {
            auto raw = elit21::Json::parse(payload.getString("payload"));
            if (!raw) return elit21::Result<void>::failure("Invalid inventory payload: " + raw.error());
            elit21::Json follow_up = elit21::Json::object();
            follow_up.set("reason", "shopify_inventory_webhook");
            follow_up.set("inventory_item_id", raw.value().getScalarString("inventory_item_id"));
            follow_up.set("location_id", raw.value().getScalarString("location_id"));
            auto queued = database.enqueueUniqueTask(
                "inventory_sync", follow_up.dump(), 20, {},
                "inventory-sync:" + raw.value().getScalarString("inventory_item_id"));
            if (!queued) return elit21::Result<void>::failure(queued.error());
            return database.audit("INFO", "shopify", "Inventory reconciliation queued", follow_up.dump());
        }
        if (task.kind == "shopify_fulfillment_updated") {
            elit21::Json follow_up = elit21::Json::object();
            follow_up.set("reason", "shopify_fulfillment_webhook");
            follow_up.set("source_webhook_id", payload.getString("webhook_id"));
            auto queued = database.enqueueUniqueTask(
                "tracking_sync", follow_up.dump(), 25, {}, "tracking-sync:shopify");
            if (!queued) return elit21::Result<void>::failure(queued.error());
            return database.audit("INFO", "shopify", "Fulfillment reconciliation queued", follow_up.dump());
        }
        if (task.kind == "shopify_app_uninstalled") {
            elit21::Json state = elit21::Json::object();
            state.set("installed", false);
            state.set("reason", "app_uninstalled_webhook");
            state.set("webhook_id", payload.getString("webhook_id"));
            auto stored = database.setRuntimeState("shopify.connection", state.dump());
            if (!stored) return stored;
            return database.audit("WARNING", "shopify", "Shopify application uninstalled", state.dump());
        }
        if (task.kind == "shopify_customers_data_request" ||
            task.kind == "shopify_customers_redact" || task.kind == "shopify_shop_redact") {
            auto processed = privacy.processTask(task.kind, payload);
            return processed ? elit21::Result<void>::success()
                             : elit21::Result<void>::failure(processed.error());
        }
        if (task.kind == "review_shopify_order") {
            const auto order_id = payload.getString("shopify_order_id");
            if (order_id.empty()) return elit21::Result<void>::failure("Review task has no Shopify order id");
            return database.audit("WARNING", "shopify.order.review",
                                  "Shopify order remains in manual review", payload.dump(), order_id);
        }
        if (task.kind == "shopify_webhook_unknown") {
            return database.audit("WARNING", "shopify", "Unknown Shopify webhook topic retained", payload.dump());
        }
        if (task.kind == "shopify_reconcile_resource") {
            const auto resource_type = payload.getString("resource_type");
            const auto resource_id = payload.getString("resource_id");
            const auto dedupe_key = payload.getString("dedupe_key");
            if (!dedupe_key.empty()) {
                auto running = database.updateShopifyReconciliationStatus(
                    dedupe_key, "running");
                if (!running) return running;
            }
            const auto next_kind =
                elit21::shopify::ShopifyReconciliationCoordinator::taskKindForResource(resource_type);
            if (next_kind == "shopify_reconcile_unknown") {
                if (!dedupe_key.empty()) {
                    database.updateShopifyReconciliationStatus(
                        dedupe_key, "failed", "unknown_resource_type");
                }
                return database.audit(
                    "WARNING", "shopify.reconciliation",
                    "Unknown Shopify resource retained for manual reconciliation",
                    payload.dump(), resource_id);
            }
            elit21::Json follow_up = payload;
            follow_up.set("reconciliation_task", true);
            follow_up.set("reconciliation_dedupe_key", dedupe_key);
            const auto source_identity = payload.getString("source_event_id").empty()
                ? payload.getString("source_webhook_id")
                : payload.getString("source_event_id");
            auto queued = database.enqueueUniqueTask(
                next_kind, follow_up.dump(), 15, {},
                "shopify-reconcile-followup:" + next_kind + ":" + resource_id + ":" + source_identity);
            if (!queued) {
                if (!dedupe_key.empty()) {
                    database.updateShopifyReconciliationStatus(
                        dedupe_key, "failed", queued.error());
                }
                return elit21::Result<void>::failure(queued.error());
            }
            return database.audit(
                "INFO", "shopify.reconciliation",
                "Shopify reconciliation follow-up queued",
                follow_up.dump(), resource_id);
        }
        if (task.kind == "shopify_reconcile_webhooks") {
            if (!shopifyReady()) {
                if (reconciliation_task) {
                    return elit21::Result<void>::failure(
                        "Webhook reconciliation requires Shopify credentials");
                }
                return database.audit(
                    "INFO", "shopify.reconciliation",
                    "Webhook reconciliation skipped: Shopify credentials unavailable");
            }
            elit21::ShopifyWebhookRegistry registry(shopify, config.shopify);
            auto reconciled = registry.reconcile(config.app.dry_run);
            if (!reconciled) {
                return elit21::Result<void>::failure(reconciled.error());
            }
            return database.audit(
                "INFO", "shopify.reconciliation",
                "Shopify webhook subscriptions reconciled",
                reconciled.value().toJson().dump());
        }
        if (task.kind == "place_supplier_order") {
            orders.processPending();
        } else if (task.kind == "catalog_sync") {
            if (!shopifyReady() || !aliexpressReady()) {
                if (reconciliation_task) return elit21::Result<void>::failure(
                    "catalog_sync reconciliation requires Shopify and AliExpress credentials");
                return database.audit("INFO", "worker", "catalog_sync skipped: API credentials unavailable");
            }
            catalog.synchronize();
        } else if (task.kind == "inventory_sync") {
            if (!shopifyReady() || !aliexpressReady()) {
                if (reconciliation_task) return elit21::Result<void>::failure(
                    "inventory_sync reconciliation requires Shopify and AliExpress credentials");
                return database.audit("INFO", "worker", "inventory_sync skipped: API credentials unavailable");
            }
            inventory.synchronize();
        } else if (task.kind == "order_poll") {
            if (!shopifyReady()) {
                if (reconciliation_task) return elit21::Result<void>::failure(
                    "order_poll reconciliation requires Shopify credentials");
                return database.audit("INFO", "worker", "order_poll skipped: Shopify credentials unavailable");
            }
            orders.pollShopify();
            orders.processPending();
        } else if (task.kind == "tracking_sync") {
            if (!shopifyReady() || !aliexpressReady()) {
                if (reconciliation_task) return elit21::Result<void>::failure(
                    "tracking_sync reconciliation requires Shopify and AliExpress credentials");
                return database.audit("INFO", "worker", "tracking_sync skipped: API credentials unavailable");
            }
            fulfillment.synchronize();
        } else if (task.kind == "daily_report") {
            return reports.write(config.app.data_dir + "/reports", counters);
        } else {
            return elit21::Result<void>::failure("Unsupported task kind: " + task.kind);
        }

        if (counters.errors > before_errors) {
            return elit21::Result<void>::failure(
                task.kind + " reported " + std::to_string(counters.errors - before_errors) + " runtime error(s)");
        }
        return elit21::Result<void>::success();
    }

    elit21::Config config;
    elit21::RuntimeCounters counters;
    elit21::Logger logger;
    elit21::HttpClient http;
    elit21::Database database;
    elit21::CanadaCompliance compliance;
    elit21::RiskEngine risk;
    elit21::PricingEngine pricing;
    elit21::ShopifyClient shopify;
    elit21::AliExpressClient aliexpress;
    elit21::SourcingEngine sourcing;
    elit21::CatalogManager catalog;
    elit21::InventoryManager inventory;
    elit21::OrderManager orders;
    elit21::FulfillmentManager fulfillment;
    elit21::ReportGenerator reports;
    elit21::shopify::ShopifyPrivacyService privacy;
    elit21::ShopifyWebhook webhook;
};

int retryDelaySeconds(int attempts) {
    const int exponent = std::clamp(attempts, 0, 8);
    return std::min(3600, 15 * (1 << exponent));
}

} // namespace

int main(int argc, char** argv) {
    const std::string config_path = argc > 1 ? argv[1] : "config/app.json";
    auto loaded = elit21::Config::load(config_path);
    if (!loaded) {
        std::cerr << loaded.error() << '\n';
        return 2;
    }

    WorkerRuntime runtime(loaded.take());
    auto initialized = runtime.initialize();
    if (!initialized) {
        std::cerr << initialized.error() << '\n';
        return 3;
    }

    const int limit = argc > 2 ? std::clamp(std::atoi(argv[2]), 1, 200) : 20;
    const std::string worker_id = "worker-" + elit21::crypto::randomHex(8);
    auto tasks = runtime.database.claimTasks(worker_id, limit);
    if (!tasks) {
        std::cerr << tasks.error() << '\n';
        return 4;
    }

    int completed = 0;
    int failed = 0;
    for (const auto& task : tasks.value()) {
        std::cout << "TASK " << task.id << ' ' << task.kind << " attempt=" << task.attempts + 1 << '\n';
        auto payload = elit21::Json::parse(task.payload_json);
        elit21::Result<void> operation = payload
            ? runtime.execute(task, payload.value())
            : elit21::Result<void>::failure("Invalid task JSON: " + payload.error());

        if (operation) {
            const auto reconciliation_key = payload
                ? payload.value().getString("reconciliation_dedupe_key") : std::string{};
            if (!reconciliation_key.empty()) {
                auto marked_reconciliation = runtime.database.updateShopifyReconciliationStatus(
                    reconciliation_key, "completed");
                if (!marked_reconciliation) {
                    operation = elit21::Result<void>::failure(marked_reconciliation.error());
                }
            }
            const auto webhook_id = payload ? payload.value().getString("webhook_id") : std::string{};
            if (!webhook_id.empty()) {
                auto marked = runtime.database.markWebhookProcessed("shopify", webhook_id);
                if (!marked) {
                    operation = elit21::Result<void>::failure(marked.error());
                }
            }
        }
        if (operation) {
            auto done = runtime.database.completeTask(task.id);
            if (!done) {
                std::cerr << "TASK " << task.id << " completion storage error: " << done.error() << '\n';
                ++failed;
                continue;
            }
            ++completed;
            continue;
        }

        const auto reconciliation_key = payload
            ? payload.value().getString("reconciliation_dedupe_key") : std::string{};
        if (!reconciliation_key.empty()) {
            runtime.database.updateShopifyReconciliationStatus(
                reconciliation_key, "failed", operation.error());
        }
        const auto webhook_id = payload ? payload.value().getString("webhook_id") : std::string{};
        const bool terminal = task.attempts + 1 >= task.max_attempts;
        if (!webhook_id.empty()) {
            if (terminal) runtime.database.markWebhookProcessed("shopify", webhook_id, operation.error());
            else runtime.database.markWebhookRetrying("shopify", webhook_id, operation.error());
        }
        auto stored = runtime.database.failTask(task.id, operation.error(), retryDelaySeconds(task.attempts));
        if (!stored) std::cerr << "TASK " << task.id << " failure storage error: " << stored.error() << '\n';
        runtime.logger.error("worker", "Task " + std::to_string(task.id) + " failed: " + operation.error());
        std::cerr << "TASK " << task.id << " FAILED: " << operation.error() << '\n';
        ++failed;
    }

    runtime.database.metric("worker.tasks.completed", completed, "{\"worker_id\":\"" + worker_id + "\"}");
    runtime.database.metric("worker.tasks.failed", failed, "{\"worker_id\":\"" + worker_id + "\"}");
    std::cout << "claimed=" << tasks.value().size() << " completed=" << completed << " failed=" << failed << '\n';
    return failed == 0 ? 0 : 5;
}
