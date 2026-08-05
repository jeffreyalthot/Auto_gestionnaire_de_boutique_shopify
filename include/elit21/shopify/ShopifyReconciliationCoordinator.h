#pragma once

#include "elit21/core/Result.h"
#include "elit21/json/Json.h"
#include "elit21/storage/Database.h"

#include <string>
#include <vector>

namespace elit21::shopify {

struct ShopifyReconciliationScheduleResult {
    int inserted{0};
    int deduplicated{0};
    std::vector<std::string> task_kinds;
    Json toJson() const;
};

class ShopifyReconciliationCoordinator {
public:
    ShopifyReconciliationCoordinator(Database& database, std::string shop_domain)
        : database_(database), shop_domain_(std::move(shop_domain)) {}

    Result<TaskEnqueueResult> requestResource(
        const std::string& resource_type,
        const std::string& resource_id,
        const std::string& reason,
        const std::string& source_event_id = {},
        const std::string& source_webhook_id = {},
        const std::string& requested_from = {});
    Result<ShopifyReconciliationScheduleResult> schedulePeriodic(
        bool catalog,
        bool inventory,
        bool orders,
        bool fulfillment,
        bool webhooks,
        const std::string& schedule_bucket = {});

    static std::string taskKindForResource(const std::string& resource_type);
    static std::string currentHourlyBucket();

private:
    Database& database_;
    std::string shop_domain_;
};

} // namespace elit21::shopify
