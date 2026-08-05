#pragma once

#include "elit21/aliexpress/AliExpressClient.h"
#include "elit21/config/Config.h"
#include "elit21/core/Types.h"
#include "elit21/logging/Logger.h"
#include "elit21/pricing/PricingEngine.h"
#include "elit21/shopify/ShopifyClient.h"
#include "elit21/storage/Database.h"

namespace elit21 {

class InventoryManager {
public:
    InventoryManager(AliExpressClient& aliexpress,
                     ShopifyClient& shopify,
                     PricingEngine& pricing,
                     Database& database,
                     Logger& logger,
                     Config& config,
                     RuntimeCounters& counters)
        : ali_(aliexpress), shop_(shopify), pricing_(pricing), db_(database), log_(logger),
          config_(config), counters_(counters) {}
    void synchronize();

private:
    AliExpressClient& ali_;
    ShopifyClient& shop_;
    PricingEngine& pricing_;
    Database& db_;
    Logger& log_;
    Config& config_;
    RuntimeCounters& counters_;
};

} // namespace elit21
