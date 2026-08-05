#pragma once
#include "elit21/aliexpress/AliExpressClient.h"
#include "elit21/compliance/CanadaCompliance.h"
#include "elit21/config/Config.h"
#include "elit21/core/Types.h"
#include "elit21/logging/Logger.h"
#include "elit21/risk/RiskEngine.h"
#include "elit21/shopify/ShopifyClient.h"
#include "elit21/shopify/ShopifyOrderAdmissionService.h"
#include "elit21/storage/Database.h"
namespace elit21 {class OrderManager{public:OrderManager(ShopifyClient&s,AliExpressClient&a,CanadaCompliance&c,RiskEngine&r,Database&d,Logger&l,Config&cfg,RuntimeCounters&n):shop_(s),ali_(a),compliance_(c),risk_(r),db_(d),log_(l),config_(cfg),admission_(d,cfg.pricing,cfg.inventory,cfg.risk),counters_(n){}void pollShopify();Result<void> ingest(CustomerOrder order);void processPending();private:Result<void> resolveMappings(CustomerOrder&order);std::string chooseShipping(const CustomerOrder&order);ShopifyClient&shop_;AliExpressClient&ali_;CanadaCompliance&compliance_;RiskEngine&risk_;Database&db_;Logger&log_;Config&config_;shopify::ShopifyOrderAdmissionService admission_;RuntimeCounters&counters_;};}
