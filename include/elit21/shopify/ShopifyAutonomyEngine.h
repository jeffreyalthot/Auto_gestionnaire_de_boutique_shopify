#pragma once

#include "elit21/config/Config.h"
#include "elit21/core/Result.h"
#include "elit21/json/Json.h"
#include "elit21/shopify/ShopifyClient.h"
#include "elit21/shopify/ShopifyOrderAdmissionService.h"
#include "elit21/shopify/ShopifyWebhookRegistry.h"
#include "elit21/storage/Database.h"

#include <cstdint>
#include <string>
#include <vector>

namespace elit21 {

struct ShopifyAutonomyReport {
    bool dry_run{true};
    bool credentials_ready{false};
    bool shop_reachable{false};
    int paid_orders_seen{0};
    int new_orders_persisted{0};
    int duplicate_orders{0};
    int tasks_enqueued{0};
    int webhook_required{0};
    int webhook_existing{0};
    int webhook_created{0};
    int webhook_planned{0};
    int webhook_failed{0};
    std::uint64_t graphql_requests{0};
    std::uint64_t graphql_retries{0};
    std::uint64_t graphql_throttles{0};
    std::vector<std::string> warnings;

    [[nodiscard]] Json toJson() const;
    [[nodiscard]] bool healthy() const;
};

class ShopifyAutonomyEngine {
public:
    ShopifyAutonomyEngine(ShopifyClient& client,
                          Database& database,
                          ShopifyConfig config,
                          PricingConfig pricing = {},
                          InventoryConfig inventory = {},
                          RiskConfig risk = {});

    Result<ShopifyAutonomyReport> run(bool dry_run,
                                      int maximum_orders = 250,
                                      bool reconcile_webhooks = true);
    Result<int> ingestPaidOrders(const std::vector<CustomerOrder>& orders,
                                 bool dry_run,
                                 ShopifyAutonomyReport& report);
    Result<void> validateConfiguration(bool live_mode) const;

private:
    ShopifyClient& client_;
    Database& database_;
    ShopifyConfig config_;
    shopify::ShopifyOrderAdmissionService admission_;
};

} // namespace elit21
