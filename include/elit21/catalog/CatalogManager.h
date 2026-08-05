#pragma once
#include "elit21/aliexpress/AliExpressClient.h"
#include "elit21/config/Config.h"
#include "elit21/core/Types.h"
#include "elit21/logging/Logger.h"
#include "elit21/pricing/PricingEngine.h"
#include "elit21/shopify/ShopifyClient.h"
#include "elit21/sourcing/SourcingEngine.h"
#include "elit21/storage/Database.h"
namespace elit21 {class CatalogManager{public:CatalogManager(AliExpressClient&a,ShopifyClient&s,PricingEngine&p,SourcingEngine&se,Database&d,Logger&l,Config&c,RuntimeCounters&n):ali_(a),shop_(s),pricing_(p),sourcing_(se),db_(d),log_(l),config_(c),counters_(n){}void synchronize();Result<void> importProduct(const AliProduct&p,double shipping);private:AliExpressClient&ali_;ShopifyClient&shop_;PricingEngine&pricing_;SourcingEngine&sourcing_;Database&db_;Logger&log_;Config&config_;RuntimeCounters&counters_;};}
