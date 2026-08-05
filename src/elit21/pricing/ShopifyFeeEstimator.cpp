#include "elit21/pricing/ShopifyFeeEstimator.h"

#include "elit21/shopify/ShopifyMoney.h"

#include <cmath>

namespace elit21::pricing {

Result<void> ShopifyFeePolicy::validate() const {
    if (!std::isfinite(payment_percentage) || !std::isfinite(payment_fixed_cad) ||
        !std::isfinite(platform_transaction_percentage) || !std::isfinite(reserve_percentage))
        return Result<void>::failure("Shopify fee policy contains a non-finite value");
    if (payment_percentage < 0.0 || payment_percentage > 100.0 || payment_fixed_cad < 0.0 ||
        platform_transaction_percentage < 0.0 || platform_transaction_percentage > 100.0 ||
        reserve_percentage < 0.0 || reserve_percentage > 100.0)
        return Result<void>::failure("Shopify fee policy is outside the accepted bounds");
    return Result<void>::success();
}

Json ShopifyFeeEstimate::toJson() const {
    Json output = Json::object();
    output.set("gross_cad", gross_cad);
    output.set("payment_fee_cad", payment_fee_cad);
    output.set("platform_fee_cad", platform_fee_cad);
    output.set("reserve_cad", reserve_cad);
    output.set("total_fees_cad", total_fees_cad);
    output.set("net_after_fees_cad", net_after_fees_cad);
    return output;
}

ShopifyFeeEstimator::ShopifyFeeEstimator()
    : platform::BusinessComponent(
          "ShopifyFeeEstimator",
          "configurable Shopify payment and transaction fee estimator",
          platform::BusinessComponentSpec{"pricing", "shopify_fee_estimate", {}, false, false, 64U * 1024U}) {}

Result<ShopifyFeeEstimate> ShopifyFeeEstimator::estimate(double gross_cad,
                                                          const ShopifyFeePolicy& policy) {
    if (!std::isfinite(gross_cad) || gross_cad < 0.0)
        return Result<ShopifyFeeEstimate>::failure("Gross Shopify amount is invalid");
    auto validation = policy.validate();
    if (!validation) return Result<ShopifyFeeEstimate>::failure(validation.error());
    auto gross = shopify::ShopifyMoney::fromDouble(gross_cad);
    if (!gross) return Result<ShopifyFeeEstimate>::failure(gross.error());
    const auto percentCents = [&](double percentage) {
        return static_cast<std::int64_t>(std::llround(
            static_cast<long double>(gross.value().cents()) * percentage / 100.0L));
    };
    auto fixed = shopify::ShopifyMoney::fromDouble(policy.payment_fixed_cad);
    if (!fixed) return Result<ShopifyFeeEstimate>::failure(fixed.error());
    const shopify::ShopifyMoney payment(percentCents(policy.payment_percentage) + fixed.value().cents());
    const shopify::ShopifyMoney platform(percentCents(policy.platform_transaction_percentage));
    const shopify::ShopifyMoney reserve(percentCents(policy.reserve_percentage));
    const auto total = payment + platform + reserve;
    ShopifyFeeEstimate output;
    output.gross_cad = gross.value().toDouble();
    output.payment_fee_cad = payment.toDouble();
    output.platform_fee_cad = platform.toDouble();
    output.reserve_cad = reserve.toDouble();
    output.total_fees_cad = total.toDouble();
    output.net_after_fees_cad = (gross.value() - total).toDouble();
    return Result<ShopifyFeeEstimate>::success(output);
}

platform::OperationResult ShopifyFeeEstimator::execute(const platform::OperationContext& context) {
    auto base = executeBusiness(context);
    if (!base.success) return base;
    auto document = Json::parse(context.payload.empty() ? "{}" : context.payload);
    if (!document) return platform::OperationResult::failure("INVALID_JSON", document.error());
    ShopifyFeePolicy policy;
    policy.payment_percentage = document.value().getNumber("payment_percentage", 0.0);
    policy.payment_fixed_cad = document.value().getNumber("payment_fixed_cad", 0.0);
    policy.platform_transaction_percentage = document.value().getNumber("platform_transaction_percentage", 0.0);
    policy.reserve_percentage = document.value().getNumber("reserve_percentage", 0.0);
    auto estimated = estimate(document.value().getNumber("gross_cad", 0.0), policy);
    if (!estimated) return platform::OperationResult::failure("INVALID_FEE_POLICY", estimated.error());
    base.message = "Shopify fees estimated from explicit configurable rates";
    base.metrics["gross_cad"] = estimated.value().gross_cad;
    base.metrics["total_fees_cad"] = estimated.value().total_fees_cad;
    base.metrics["net_after_fees_cad"] = estimated.value().net_after_fees_cad;
    base.attributes["fee_source"] = "configuration_not_hardcoded";
    return base;
}

} // namespace elit21::pricing
