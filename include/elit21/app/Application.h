#pragma once

#include "elit21/aliexpress/AliExpressClient.h"
#include "elit21/automation/AutomationEngine.h"
#include "elit21/catalog/CatalogManager.h"
#include "elit21/compliance/CanadaCompliance.h"
#include "elit21/config/Config.h"
#include "elit21/core/Result.h"
#include "elit21/fulfillment/FulfillmentManager.h"
#include "elit21/inventory/InventoryManager.h"
#include "elit21/logging/Logger.h"
#include "elit21/net/HttpClient.h"
#include "elit21/net/HttpServer.h"
#include "elit21/orders/OrderManager.h"
#include "elit21/pricing/PricingEngine.h"
#include "elit21/reports/ReportGenerator.h"
#include "elit21/risk/RiskEngine.h"
#include "elit21/shopify/ShopifyClient.h"
#include "elit21/shopify/ShopifyWebhookProcessor.h"
#include "elit21/sourcing/SourcingEngine.h"
#include "elit21/storage/Database.h"
#include "elit21/terminal/Dashboard.h"

namespace elit21 {

class Application {
public:
    explicit Application(Config config);
    Result<void> initialize();
    int run();
    void stop();

private:
    OutgoingResponse handleHttp(const IncomingRequest& request);

    Config config_;
    RuntimeCounters counters_;
    Logger logger_;
    HttpClient http_;
    Database db_;
    CanadaCompliance compliance_;
    RiskEngine risk_;
    PricingEngine pricing_;
    ShopifyClient shopify_;
    AliExpressClient aliexpress_;
    SourcingEngine sourcing_;
    CatalogManager catalog_;
    InventoryManager inventory_;
    OrderManager orders_;
    FulfillmentManager fulfillment_;
    ReportGenerator reports_;
    Dashboard dashboard_;
    AutomationEngine automation_;
    ShopifyWebhookProcessor webhook_processor_;
    HttpServer server_;
};

} // namespace elit21
