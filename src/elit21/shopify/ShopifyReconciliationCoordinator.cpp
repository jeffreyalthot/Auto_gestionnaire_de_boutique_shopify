#include "elit21/shopify/ShopifyReconciliationCoordinator.h"

#include "elit21/security/Crypto.h"
#include "elit21/util/StringUtil.h"
#include "elit21/util/TimeUtil.h"

#include <array>

namespace elit21::shopify {

Json ShopifyReconciliationScheduleResult::toJson() const {
    Json output = Json::object();
    output.set("inserted", inserted);
    output.set("deduplicated", deduplicated);
    Json kinds = Json::array();
    for (const auto& kind : task_kinds) {
        Json value = Json::object();
        value.set("kind", kind);
        kinds.push(value);
    }
    output.set("task_kinds", kinds);
    return output;
}

Result<TaskEnqueueResult> ShopifyReconciliationCoordinator::requestResource(
    const std::string& resource_type,
    const std::string& resource_id,
    const std::string& reason,
    const std::string& source_event_id,
    const std::string& source_webhook_id,
    const std::string& requested_from) {
    if (shop_domain_.empty()) {
        return Result<TaskEnqueueResult>::failure(
            "Shopify reconciliation shop domain is empty");
    }
    return database_.requestShopifyReconciliation(
        shop_domain_, util::lower(resource_type), resource_id, reason,
        source_event_id, source_webhook_id, requested_from);
}

Result<ShopifyReconciliationScheduleResult>
ShopifyReconciliationCoordinator::schedulePeriodic(
    bool catalog,
    bool inventory,
    bool orders,
    bool fulfillment,
    bool webhooks,
    const std::string& schedule_bucket) {
    if (shop_domain_.empty()) {
        return Result<ShopifyReconciliationScheduleResult>::failure(
            "Shopify reconciliation shop domain is empty");
    }
    struct Entry {
        const char* kind;
        int priority;
        bool enabled;
    };
    const std::array<Entry, 5> entries{{
        {"catalog_sync", 50, catalog},
        {"inventory_sync", 40, inventory},
        {"order_poll", 20, orders},
        {"tracking_sync", 30, fulfillment},
        {"shopify_reconcile_webhooks", 10, webhooks},
    }};
    ShopifyReconciliationScheduleResult output;
    const auto bucket = schedule_bucket.empty() ? currentHourlyBucket() : schedule_bucket;
    for (const auto& entry : entries) {
        if (!entry.enabled) continue;
        Json payload = Json::object();
        payload.set("shop_domain", shop_domain_);
        payload.set("scheduled", true);
        payload.set("schedule_bucket", bucket);
        payload.set("scheduled_at", util::utcNowIso());
        auto task = database_.enqueueUniqueTask(
            entry.kind, payload.dump(), entry.priority, {},
            "shopify-periodic:" + shop_domain_ + ":" + entry.kind + ":" + bucket);
        if (!task) {
            return Result<ShopifyReconciliationScheduleResult>::failure(task.error());
        }
        output.task_kinds.emplace_back(entry.kind);
        if (task.value().inserted) ++output.inserted;
        else ++output.deduplicated;
    }
    database_.audit(
        "INFO", "shopify.reconciliation", "Periodic Shopify reconciliation scheduled",
        output.toJson().dump());
    return Result<ShopifyReconciliationScheduleResult>::success(std::move(output));
}

std::string ShopifyReconciliationCoordinator::taskKindForResource(
    const std::string& resource_type) {
    const auto value = util::lower(resource_type);
    if (value == "orders" || value == "order" || value == "refunds" || value == "returns") {
        return "order_poll";
    }
    if (value == "products" || value == "product" || value == "collections") {
        return "catalog_sync";
    }
    if (value == "inventory_levels" || value == "inventory" ||
        value == "inventory_items") {
        return "inventory_sync";
    }
    if (value == "fulfillments" || value == "fulfillment_orders") {
        return "tracking_sync";
    }
    if (value == "webhooks" || value == "subscriptions") {
        return "shopify_reconcile_webhooks";
    }
    return "shopify_reconcile_unknown";
}

std::string ShopifyReconciliationCoordinator::currentHourlyBucket() {
    const auto now = util::utcNowIso();
    return now.size() >= 13 ? now.substr(0, 13) : now;
}

} // namespace elit21::shopify
