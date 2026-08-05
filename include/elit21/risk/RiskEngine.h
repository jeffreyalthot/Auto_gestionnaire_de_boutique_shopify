#pragma once
#include "elit21/config/Config.h"
#include "elit21/core/Result.h"
#include "elit21/core/Types.h"
namespace elit21 {class RiskEngine{public:explicit RiskEngine(RiskConfig c):config_(c){}Result<void> approveOrder(const CustomerOrder&o)const;Result<void> approveSupplier(double seller_score,double rating,int orders,int delivery_days,const SourcingConfig&s)const;private:RiskConfig config_;};}
