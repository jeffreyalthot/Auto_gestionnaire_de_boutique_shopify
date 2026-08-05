#pragma once
#include "elit21/aliexpress/AliExpressClient.h"
#include "elit21/core/Types.h"
#include "elit21/logging/Logger.h"
#include "elit21/shopify/ShopifyClient.h"
#include "elit21/storage/Database.h"
namespace elit21 {class FulfillmentManager{public:FulfillmentManager(AliExpressClient&a,ShopifyClient&s,Database&d,Logger&l,RuntimeCounters&n):ali_(a),shop_(s),db_(d),log_(l),counters_(n){}void synchronize();private:AliExpressClient&ali_;ShopifyClient&shop_;Database&db_;Logger&log_;RuntimeCounters&counters_;};}
