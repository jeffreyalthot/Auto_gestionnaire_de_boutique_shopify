#include "elit21/pricing/PricingEngine.h"

#include "elit21/shopify/ShopifyMoney.h"

#include <algorithm>
#include <cmath>

namespace elit21 {

Result<PriceBreakdown> PricingEngine::calculate(double supplier_cost_cad, double shipping_cad) const {
    if (!std::isfinite(supplier_cost_cad) || !std::isfinite(shipping_cad) ||
        supplier_cost_cad < 0.0 || shipping_cad < 0.0) {
        return Result<PriceBreakdown>::failure("Coût fournisseur ou livraison invalide.");
    }
    if (!std::isfinite(config_.markup_percent_before_shipping) ||
        config_.markup_percent_before_shipping < 100.0) {
        return Result<PriceBreakdown>::failure("La marge avant livraison doit être d'au moins 100%.");
    }
    auto cost = shopify::ShopifyMoney::fromDouble(supplier_cost_cad);
    auto shipping = shopify::ShopifyMoney::fromDouble(shipping_cad);
    if (!cost || !shipping) return Result<PriceBreakdown>::failure("Montant impossible à représenter en centimes.");
    auto final = shopify::ShopifyMoney::supplierPrice(
        cost.value(), config_.markup_percent_before_shipping, shipping.value());
    if (!final) return Result<PriceBreakdown>::failure(final.error());

    const auto markup_cents = static_cast<std::int64_t>(std::llround(
        static_cast<long double>(cost.value().cents()) *
        static_cast<long double>(config_.markup_percent_before_shipping) / 100.0L));
    const shopify::ShopifyMoney markup(markup_cents);
    shopify::ShopifyMoney before_shipping = cost.value() + markup;
    shopify::ShopifyMoney adjusted_final = final.value();
    const auto minimum = shopify::ShopifyMoney::fromDouble(std::max(0.0, config_.minimum_price_cad));
    const auto maximum = shopify::ShopifyMoney::fromDouble(config_.maximum_price_cad);
    if (!minimum || !maximum || maximum.value() <= minimum.value())
        return Result<PriceBreakdown>::failure("Bornes de prix invalides.");
    if (adjusted_final < minimum.value()) adjusted_final = minimum.value();
    if (adjusted_final > maximum.value())
        return Result<PriceBreakdown>::failure("Prix final supérieur au maximum configuré.");

    PriceBreakdown breakdown;
    breakdown.supplier_cost_cad = cost.value().toDouble();
    breakdown.markup_amount_cad = markup.toDouble();
    breakdown.gross_profit_before_shipping_cad = markup.toDouble();
    breakdown.price_before_shipping_cad = before_shipping.toDouble();
    breakdown.shipping_cad = shipping.value().toDouble();
    breakdown.final_price_cad = adjusted_final.toDouble();
    return Result<PriceBreakdown>::success(breakdown);
}

} // namespace elit21
