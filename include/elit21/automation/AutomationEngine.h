#pragma once
#include "elit21/catalog/CatalogManager.h"
#include "elit21/config/Config.h"
#include "elit21/fulfillment/FulfillmentManager.h"
#include "elit21/inventory/InventoryManager.h"
#include "elit21/orders/OrderManager.h"
#include "elit21/reports/ReportGenerator.h"
#include "elit21/terminal/Dashboard.h"
#include <atomic>
namespace elit21 {class AutomationEngine{public:AutomationEngine(Config&c,CatalogManager&cat,InventoryManager&i,OrderManager&o,FulfillmentManager&f,ReportGenerator&r,Dashboard&d,RuntimeCounters&n):config_(c),catalog_(cat),inventory_(i),orders_(o),fulfillment_(f),reports_(r),dashboard_(d),counters_(n){}void run();void requestStop(){stop_=true;}private:Config&config_;CatalogManager&catalog_;InventoryManager&inventory_;OrderManager&orders_;FulfillmentManager&fulfillment_;ReportGenerator&reports_;Dashboard&dashboard_;RuntimeCounters&counters_;std::atomic_bool stop_{false};};}
