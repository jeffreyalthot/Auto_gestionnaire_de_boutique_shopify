#include "elit21/shopify/ShopifyAutonomyEngine.h"

#include "elit21/security/Crypto.h"
#include "elit21/shopify/ShopifyEndpoints.h"
#include <utility>

namespace elit21 {

Json ShopifyAutonomyReport::toJson() const {
    Json output = Json::object();
    output.set("dry_run", dry_run);
    output.set("credentials_ready", credentials_ready);
    output.set("shop_reachable", shop_reachable);
    output.set("paid_orders_seen", paid_orders_seen);
    output.set("new_orders_persisted", new_orders_persisted);
    output.set("duplicate_orders", duplicate_orders);
    output.set("tasks_enqueued", tasks_enqueued);
    output.set("webhook_required", webhook_required);
    output.set("webhook_existing", webhook_existing);
    output.set("webhook_created", webhook_created);
    output.set("webhook_planned", webhook_planned);
    output.set("webhook_failed", webhook_failed);
    output.set("graphql_requests", static_cast<std::int64_t>(graphql_requests));
    output.set("graphql_retries", static_cast<std::int64_t>(graphql_retries));
    output.set("graphql_throttles", static_cast<std::int64_t>(graphql_throttles));
    Json warning_list = Json::array();
    for (const auto& warning : warnings) {
        Json item = Json::object();
        item.set("message", warning);
        warning_list.push(item);
    }
    output.set("warnings", warning_list);
    return output;
}

bool ShopifyAutonomyReport::healthy() const {
    return credentials_ready && shop_reachable && webhook_failed == 0;
}

ShopifyAutonomyEngine::ShopifyAutonomyEngine(ShopifyClient& client,
                                             Database& database,
                                             ShopifyConfig config,
                                             PricingConfig pricing,
                                             InventoryConfig inventory,
                                             RiskConfig risk)
    : client_(client), database_(database), config_(std::move(config)),
      admission_(database_, std::move(pricing), std::move(inventory), std::move(risk)) {}

Result<void> ShopifyAutonomyEngine::validateConfiguration(bool live_mode) const {
    if (!shopify::ShopifyEndpoints::validateShop(config_.shop)) {
        return Result<void>::failure("Shopify shop name is invalid");
    }
    if (config_.access_token.empty()) return Result<void>::failure("Shopify access token is missing");
    if (live_mode && config_.webhook_secret.size() < 16) {
        return Result<void>::failure("Shopify webhook secret is missing or too short");
    }
    if (live_mode && !shopify::ShopifyEndpoints::isHttpsUrl(config_.webhook_base_url)) {
        return Result<void>::failure("Shopify webhook base URL must use HTTPS in live mode");
    }
    return Result<void>::success();
}

Result<int> ShopifyAutonomyEngine::ingestPaidOrders(const std::vector<CustomerOrder>& orders,bool dry_run,ShopifyAutonomyReport& report) {int admitted_count=0;for(const auto&order:orders){auto admitted=admission_.admit(order,dry_run,true);if(!admitted){report.warnings.push_back("Shopify order "+order.shopify_order_id+" rejected: "+admitted.error());continue;}switch(admitted.value().decision){case shopify::ShopifyOrderAdmissionDecision::accepted:++admitted_count;++report.new_orders_persisted;if(admitted.value().supplier_task_enqueued)++report.tasks_enqueued;break;case shopify::ShopifyOrderAdmissionDecision::duplicate:++report.duplicate_orders;break;case shopify::ShopifyOrderAdmissionDecision::manual_review:++report.new_orders_persisted;if(admitted.value().review_task_enqueued)++report.tasks_enqueued;for(const auto&w:admitted.value().warnings)report.warnings.push_back(w);break;default:report.warnings.push_back("Shopify order rejected: "+order.shopify_order_id);break;}}return Result<int>::success(admitted_count);}

Result<ShopifyAutonomyReport> ShopifyAutonomyEngine::run(bool dry_run,
                                                         int maximum_orders,
                                                         bool reconcile_webhooks) {
    ShopifyAutonomyReport report;
    report.dry_run = dry_run;
    const std::string lease_owner = "shopify-autonomy-" + crypto::randomHex(8);
    auto lease = database_.acquireLease("shopify.autonomy", lease_owner, 900);
    if (!lease) return Result<ShopifyAutonomyReport>::failure(lease.error());
    if (!lease.value()) return Result<ShopifyAutonomyReport>::failure("Another Shopify autonomy cycle owns the lease");
    struct LeaseRelease {
        Database& database; std::string owner;
        ~LeaseRelease() { database.releaseLease("shopify.autonomy", owner); }
    } lease_release{database_, lease_owner};
    auto validation = validateConfiguration(!dry_run);
    report.credentials_ready = static_cast<bool>(validation);
    if (!validation) return Result<ShopifyAutonomyReport>::failure(validation.error());

    auto health = client_.healthCheck();
    report.shop_reachable = static_cast<bool>(health);
    if (!health) return Result<ShopifyAutonomyReport>::failure(health.error());

    auto orders = client_.fetchOpenPaidOrders(maximum_orders, 20);
    if (!orders) return Result<ShopifyAutonomyReport>::failure(orders.error());
    report.paid_orders_seen = static_cast<int>(orders.value().size());
    auto ingested = ingestPaidOrders(orders.value(), dry_run, report);
    if (!ingested) return Result<ShopifyAutonomyReport>::failure(ingested.error());

    if (reconcile_webhooks) {
        ShopifyWebhookRegistry registry(client_, config_);
        auto webhooks = registry.reconcile(dry_run);
        if (!webhooks) report.warnings.push_back(webhooks.error());
        else {
            report.webhook_required = webhooks.value().required;
            report.webhook_existing = webhooks.value().existing;
            report.webhook_created = webhooks.value().created;
            report.webhook_planned = webhooks.value().planned;
            report.webhook_failed = webhooks.value().failed;
            for (const auto& message : webhooks.value().messages) report.warnings.push_back(message);
        }
    }

    const auto metrics = client_.apiMetrics();
    report.graphql_requests = metrics.requests;
    report.graphql_retries = metrics.retries;
    report.graphql_throttles = metrics.throttles;
    database_.audit("INFO", "shopify.autonomy", report.toJson().dump());
    return Result<ShopifyAutonomyReport>::success(std::move(report));
}

} // namespace elit21
