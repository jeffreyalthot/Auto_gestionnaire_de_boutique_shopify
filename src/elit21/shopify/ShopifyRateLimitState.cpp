#include "elit21/shopify/ShopifyRateLimitState.h"

#include <algorithm>
#include <cmath>

namespace elit21::shopify {

ShopifyRateLimitState::ShopifyRateLimitState()
    : platform::BusinessComponent(
          "ShopifyRateLimitState", "Shopify GraphQL cost and throttling state",
          platform::BusinessComponentSpec{"shopify", "rate_limit_state", {}, false, false,
                                          1024U * 1024U}) {}

void ShopifyRateLimitState::update(const graphql::GraphqlTransportMetrics& metrics) {
    requests_ = metrics.requests;
    retries_ = metrics.retries;
    throttles_ = metrics.throttles;
    currently_available_ = std::clamp(metrics.currently_available_cost, 0.0, maximum_available_);
}

void ShopifyRateLimitState::update(const Json& extensions) {
    const auto throttle = extensions.get("cost").get("throttleStatus");
    maximum_available_ = std::max(1.0, throttle.getNumber("maximumAvailable", maximum_available_));
    currently_available_ = std::clamp(
        throttle.getNumber("currentlyAvailable", currently_available_), 0.0, maximum_available_);
    restore_rate_ = std::max(1.0, throttle.getNumber("restoreRate", restore_rate_));
}

double ShopifyRateLimitState::utilizationPercent() const noexcept {
    return maximum_available_ > 0.0
        ? (1.0 - currently_available_ / maximum_available_) * 100.0 : 100.0;
}

bool ShopifyRateLimitState::healthy(double minimum_available) const noexcept {
    return currently_available_ >= std::max(0.0, minimum_available);
}

std::chrono::milliseconds ShopifyRateLimitState::delayFor(double requested_cost) const {
    const auto deficit = std::max(0.0, requested_cost - currently_available_);
    return std::chrono::milliseconds(
        static_cast<long long>(std::ceil(deficit / restore_rate_ * 1000.0)));
}

Json ShopifyRateLimitState::snapshot() const {
    Json output = Json::object();
    output.set("requests", static_cast<std::int64_t>(requests_));
    output.set("retries", static_cast<std::int64_t>(retries_));
    output.set("throttles", static_cast<std::int64_t>(throttles_));
    output.set("maximum_available", maximum_available_);
    output.set("currently_available", currently_available_);
    output.set("restore_rate", restore_rate_);
    output.set("utilization_percent", utilizationPercent());
    return output;
}

platform::OperationResult ShopifyRateLimitState::execute(const platform::OperationContext&) {
    auto result = platform::OperationResult::ok("Shopify rate-limit state inspected");
    result.metrics["currently_available"] = currently_available_;
    result.metrics["utilization_percent"] = utilizationPercent();
    result.metrics["requests"] = static_cast<double>(requests_);
    result.metrics["throttles"] = static_cast<double>(throttles_);
    result.attributes["health"] = healthy() ? "healthy" : "constrained";
    return result;
}

} // namespace elit21::shopify
