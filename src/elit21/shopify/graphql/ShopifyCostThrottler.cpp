#include "elit21/shopify/graphql/ShopifyCostThrottler.h"

#include <algorithm>
#include <cmath>

namespace elit21::shopify::graphql {
namespace {

double restored(double available, double maximum, double rate,
                std::chrono::steady_clock::time_point updated) {
    const auto elapsed = std::chrono::duration<double>(std::chrono::steady_clock::now() - updated).count();
    return std::min(maximum, available + std::max(0.0, elapsed) * rate);
}

} // namespace

void ShopifyCostThrottler::update(const Json& extensions) {
    const auto cost = extensions.get("cost");
    const auto throttle = cost.get("throttleStatus");
    std::lock_guard lock(mutex_);
    requested_cost_ = std::max(0.0, cost.getNumber("requestedQueryCost", requested_cost_));
    actual_cost_ = std::max(0.0, cost.getNumber("actualQueryCost", actual_cost_));
    maximum_available_ = std::max(1.0, throttle.getNumber("maximumAvailable", maximum_available_));
    available_ = std::clamp(throttle.getNumber("currentlyAvailable", available_), 0.0, maximum_available_);
    restore_rate_ = std::max(1.0, throttle.getNumber("restoreRate", restore_rate_));
    updated_at_ = std::chrono::steady_clock::now();
}

void ShopifyCostThrottler::reserve(double estimated_cost) {
    std::lock_guard lock(mutex_);
    available_ = restored(available_, maximum_available_, restore_rate_, updated_at_);
    updated_at_ = std::chrono::steady_clock::now();
    requested_cost_ = std::max(0.0, estimated_cost);
    available_ = std::max(0.0, available_ - requested_cost_);
}

std::chrono::milliseconds ShopifyCostThrottler::recommendedDelay(double requested_cost) const {
    std::lock_guard lock(mutex_);
    const auto current = restored(available_, maximum_available_, restore_rate_, updated_at_);
    const auto deficit = std::max(0.0, std::max(0.0, requested_cost) - current);
    const auto seconds = deficit / std::max(1.0, restore_rate_);
    return std::chrono::milliseconds(static_cast<long long>(std::ceil(seconds * 1000.0)));
}

double ShopifyCostThrottler::available() const {
    std::lock_guard lock(mutex_);
    return restored(available_, maximum_available_, restore_rate_, updated_at_);
}

ShopifyCostSnapshot ShopifyCostThrottler::snapshot() const {
    std::lock_guard lock(mutex_);
    return {maximum_available_, restored(available_, maximum_available_, restore_rate_, updated_at_),
            restore_rate_, requested_cost_, actual_cost_};
}

void ShopifyCostThrottler::reset(double available, double restore_rate) {
    std::lock_guard lock(mutex_);
    maximum_available_ = std::max(1.0, available);
    available_ = maximum_available_;
    restore_rate_ = std::max(1.0, restore_rate);
    requested_cost_ = 0.0;
    actual_cost_ = 0.0;
    updated_at_ = std::chrono::steady_clock::now();
}

} // namespace elit21::shopify::graphql
