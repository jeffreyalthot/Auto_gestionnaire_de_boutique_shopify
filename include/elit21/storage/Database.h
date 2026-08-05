#pragma once

#include "elit21/core/Result.h"
#include "elit21/core/Types.h"

#include <functional>
#include <mutex>
#include <optional>
#include <sqlite3.h>
#include <string>
#include <vector>

namespace elit21 {

struct StoredProduct {
    long long id{0};
    std::string ae_id, shopify_id, title, status;
    double cost{0}, shipping{0}, price{0};
    int stock{0};
};


struct StoredVariant {
    long long id{0};
    long long product_id{0};
    std::string ae_product_id, ae_sku_id, shopify_variant_id, shopify_inventory_item_id, sku, status;
    double cost{0}, shipping{0}, price{0};
    int stock{0};
    int shopify_quantity{-1};
};

struct StoredOrder {
    long long id{0};
    std::string shopify_id, ae_id, status, payload, idempotency_key;
    std::string customer_email;
    std::string currency{"CAD"};
    double total{0};
};

struct StoredOrderLine {
    long long id{0};
    long long order_id{0};
    std::string shopify_line_id, shopify_variant_id, aliexpress_product_id, aliexpress_sku_id, sku, title;
    int quantity{0};
    double unit_price_cad{0.0};
    std::string status{"pending"};
};

struct ShopifyPrivacyRequestRecord {
    std::string request_id, topic, shop_domain, customer_id;
    std::string orders_json{"[]"};
    std::string payload_hash;
    std::string status{"received"};
};


struct ShopifyIdempotencyReservation {
    std::string idempotency_key;
    std::string operation_name;
    std::string parameter_hash;
    std::string status;
    std::string response_json;
    std::string last_error;
    std::string expires_at;
    bool inserted{false};
    [[nodiscard]] bool completed() const noexcept { return status == "completed"; }
    [[nodiscard]] bool retryable() const noexcept {
        return status == "reserved" || status == "in_progress" || status == "failed";
    }
};

enum class ShopifyWebhookOrderDecision {
    accepted,
    duplicate,
    stale
};

struct ShopifyWebhookOrderResult {
    ShopifyWebhookOrderDecision decision{ShopifyWebhookOrderDecision::accepted};
    std::string previous_triggered_at;
    std::string previous_event_id;
    std::string reason;
    [[nodiscard]] bool accepted() const noexcept {
        return decision == ShopifyWebhookOrderDecision::accepted;
    }
};

struct TaskEnqueueResult {
    long long id{0};
    bool inserted{false};
};

struct TaskRecord {
    long long id{0};
    std::string kind;
    std::string payload_json;
    std::string status;
    int priority{100};
    int attempts{0};
    int max_attempts{8};
    std::string run_after;
    std::string last_error;
};

class Database {
public:
    Database();
    ~Database();

    Database(const Database&) = delete;
    Database& operator=(const Database&) = delete;

    Result<void> open(const std::string& path);
    Result<void> migrate(const std::string& sql_path);
    Result<void> migrateDirectory(const std::string& directory);
    Result<void> execute(const std::string& sql);

    Result<void> upsertProduct(const StoredProduct& product);
    Result<std::vector<StoredProduct>> products(int limit = 100);
    Result<std::optional<StoredProduct>> productByAliExpressId(const std::string& aliexpress_product_id);
    Result<void> upsertVariant(const StoredVariant& variant);
    Result<std::vector<StoredVariant>> variants(int limit = 250);
    Result<void> updateVariantShopifyQuantity(const std::string& shopify_inventory_item_id,
                                                int quantity);
    Result<std::vector<StoredVariant>> variantsForProduct(const std::string& aliexpress_product_id);
    Result<std::optional<StoredVariant>> variantByAliExpressSku(const std::string& aliexpress_product_id,
                                                               const std::string& aliexpress_sku_id);

    Result<bool> saveOrder(const StoredOrder& order);
    Result<bool> saveOrderWithLines(const StoredOrder& order, const std::vector<StoredOrderLine>& lines);
    Result<std::vector<StoredOrderLine>> orderLines(const std::string& shopify_order_id);
    Result<std::vector<StoredOrder>> ordersByStatus(const std::string& status, int limit = 100);
    Result<std::vector<StoredOrder>> ordersByShopifyIds(const std::vector<std::string>& shopify_ids);
    Result<int> redactShopifyOrders(const std::vector<std::string>& shopify_ids,const std::string& reason);
    Result<int> redactAllShopifyCustomerData(const std::string& reason);
    Result<bool> transitionOrderStatus(const std::string& shopify_id,const std::vector<std::string>& expected_statuses,const std::string& next_status,const std::string& event_type,const std::string& context_json = "{}",const std::string& last_error = "");
    Result<void> updateOrderSupplierId(const std::string& shopify_id,
                                       const std::string& aliexpress_id,
                                       const std::string& status);
    Result<bool> setSupplierOrderAndTransition(const std::string& shopify_id,
                                               const std::string& expected_status,
                                               const std::string& aliexpress_id,
                                               const std::string& next_status,
                                               const std::string& context_json = "{}");
    Result<void> updateOrderStatus(const std::string& shopify_id,
                                   const std::string& status,
                                   const std::string& last_error = "");
    Result<void> saveShipment(long long order_id, const ShipmentInfo& shipment);

    Result<long long> enqueueTask(const std::string& kind,
                                  const std::string& payload_json,
                                  int priority = 100,
                                  const std::string& run_after = "");
    Result<TaskEnqueueResult> enqueueUniqueTask(const std::string& kind,
                                                const std::string& payload_json,
                                                int priority,
                                                const std::string& run_after,
                                                const std::string& dedupe_key);
    Result<int> recoverStaleTasks(int stale_after_seconds = 900);
    Result<std::vector<TaskRecord>> claimTasks(const std::string& worker_id, int limit = 10);
    Result<void> completeTask(long long task_id);
    Result<void> failTask(long long task_id, const std::string& error, int retry_delay_seconds = 60);

    Result<bool> recordWebhook(const std::string& provider,
                               const std::string& webhook_id,
                               const std::string& topic,
                               const std::string& payload_hash);
    Result<void> markWebhookQueued(const std::string& provider,
                                   const std::string& webhook_id);
    Result<void> markWebhookProcessing(const std::string& provider,
                                       const std::string& webhook_id);
    Result<void> markWebhookRetrying(const std::string& provider,
                                     const std::string& webhook_id,
                                     const std::string& error);
    Result<void> markWebhookProcessed(const std::string& provider,
                                      const std::string& webhook_id,
                                      const std::string& error = "");

    Result<bool> acquireLease(const std::string& lease_name,const std::string& owner_id,int ttl_seconds);
    Result<void> releaseLease(const std::string& lease_name,const std::string& owner_id);
    Result<void> recordShopifyPrivacyRequest(const ShopifyPrivacyRequestRecord& request);
    Result<void> completeShopifyPrivacyRequest(const std::string& request_id,
                                               const std::string& status,
                                               const std::string& result_path = {},
                                               const std::string& last_error = {});


    Result<ShopifyIdempotencyReservation> reserveShopifyIdempotency(
        const std::string& idempotency_key,
        const std::string& operation_name,
        const std::string& parameter_hash,
        int ttl_hours = 24);
    Result<void> markShopifyIdempotencyInProgress(const std::string& idempotency_key);
    Result<void> completeShopifyIdempotency(const std::string& idempotency_key,
                                            const std::string& response_json);
    Result<void> failShopifyIdempotency(const std::string& idempotency_key,
                                        const std::string& error);

    Result<ShopifyWebhookOrderResult> evaluateShopifyWebhookOrder(
        const std::string& shop_domain,
        const std::string& resource_type,
        const std::string& resource_id,
        const std::string& triggered_at,
        const std::string& event_id,
        const std::string& webhook_id,
        const std::string& payload_hash);
    Result<TaskEnqueueResult> requestShopifyReconciliation(
        const std::string& shop_domain,
        const std::string& resource_type,
        const std::string& resource_id,
        const std::string& reason,
        const std::string& source_event_id,
        const std::string& source_webhook_id,
        const std::string& requested_from);
    Result<void> updateShopifyReconciliationStatus(
        const std::string& dedupe_key,
        const std::string& status,
        const std::string& last_error = {});

    Result<void> recordShopifyGovernanceRun(const std::string& shop_domain,
                                              const std::string& mode,
                                              int score,
                                              bool production_ready,
                                              const std::string& api_version,
                                              const std::string& report_json);
    Result<std::optional<std::string>> latestShopifyGovernanceRun(
        const std::string& shop_domain);
    Result<void> recordShopifyApiVersionObservation(
        const std::string& shop_domain,
        const std::string& configured_version,
        const std::string& latest_supported_version,
        bool supported,
        const std::string& catalog_json);

    Result<void> recordShopifyContractAudit(
        int score,
        bool passed,
        const std::string& report_json);
    Result<void> recordShopifyCatalogSync(
        const std::string& external_product_id,
        const std::string& mode,
        const std::string& status,
        const std::string& product_gid,
        const std::string& operation_gid,
        const std::string& result_json,
        const std::string& last_error = {});

    Result<void> recordShopifyWebhookSloAudit(
        std::size_t deliveries,
        double failure_rate_percent,
        double p90_response_time_ms,
        std::size_t removed_subscriptions,
        bool healthy,
        const std::string& report_json);

    Result<void> setRuntimeState(const std::string& key, const std::string& value_json);
    Result<std::optional<std::string>> runtimeState(const std::string& key);
    Result<void> setSyncCheckpoint(const std::string& name,
                                   const std::string& cursor,
                                   const std::string& metadata_json = "{}",
                                   bool completed = false);
    Result<std::optional<std::string>> syncCheckpoint(const std::string& name);

    Result<void> audit(const std::string& level,
                       const std::string& category,
                       const std::string& message,
                       const std::string& context = "{}",
                       const std::string& correlation_id = "");
    Result<void> metric(const std::string& name, double value, const std::string& labels_json = "{}");

    bool isOpen() const { return db_ != nullptr; }

private:
    Result<void> prepareAndStep(const std::string& sql,
                                const std::function<void(sqlite3_stmt*)>& binder);
    Result<void> executeUnlocked(const std::string& sql);
    static std::string sqlQuote(const std::string& value);
    Result<void> setWebhookStatus(const std::string& provider,
                                  const std::string& webhook_id,
                                  const std::string& status,
                                  const std::string& error,
                                  bool terminal);

    sqlite3* db_{nullptr};
    mutable std::mutex mutex_;
};

} // namespace elit21
