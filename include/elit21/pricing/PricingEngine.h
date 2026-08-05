#pragma once
#include "elit21/config/Config.h"
#include "elit21/core/Result.h"
namespace elit21 {
struct PriceBreakdown { double supplier_cost_cad{0},markup_amount_cad{0},price_before_shipping_cad{0},shipping_cad{0},final_price_cad{0},gross_profit_before_shipping_cad{0}; };
class PricingEngine {public:explicit PricingEngine(PricingConfig config):config_(config){} Result<PriceBreakdown> calculate(double supplier_cost_cad,double shipping_cad)const;private:PricingConfig config_;};
}
