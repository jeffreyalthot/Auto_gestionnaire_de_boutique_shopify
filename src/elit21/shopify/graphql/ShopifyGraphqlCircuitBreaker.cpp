#include "elit21/shopify/graphql/ShopifyGraphqlCircuitBreaker.h"

#include <algorithm>

namespace elit21::shopify::graphql {

ShopifyGraphqlCircuitBreaker::ShopifyGraphqlCircuitBreaker(
    int failure_threshold,
    std::chrono::milliseconds open_duration)
    : failure_threshold_(std::max(1, failure_threshold)),
      open_duration_(std::max(std::chrono::milliseconds(1), open_duration)) {}

std::string ShopifyGraphqlCircuitBreaker::name(ShopifyCircuitState state) {
    switch (state) {
        case ShopifyCircuitState::closed: return "closed";
        case ShopifyCircuitState::open: return "open";
        case ShopifyCircuitState::half_open: return "half_open";
    }
    return "unknown";
}

void ShopifyGraphqlCircuitBreaker::refreshStateLocked(
    std::chrono::steady_clock::time_point now) {
    if (state_ == ShopifyCircuitState::open && now - opened_at_ >= open_duration_) {
        state_ = ShopifyCircuitState::half_open;
        half_open_probe_in_flight_ = false;
    }
}

bool ShopifyGraphqlCircuitBreaker::allowRequest() {
    std::lock_guard lock(mutex_);
    refreshStateLocked(std::chrono::steady_clock::now());
    if (state_ == ShopifyCircuitState::open) {
        ++rejected_requests_;
        return false;
    }
    if (state_ == ShopifyCircuitState::half_open) {
        if (half_open_probe_in_flight_) {
            ++rejected_requests_;
            return false;
        }
        half_open_probe_in_flight_ = true;
    }
    return true;
}

void ShopifyGraphqlCircuitBreaker::recordSuccess() {
    std::lock_guard lock(mutex_);
    ++successes_;
    consecutive_failures_ = 0;
    state_ = ShopifyCircuitState::closed;
    half_open_probe_in_flight_ = false;
}

void ShopifyGraphqlCircuitBreaker::recordFailure() {
    std::lock_guard lock(mutex_);
    ++failures_;
    ++consecutive_failures_;
    half_open_probe_in_flight_ = false;
    if (state_ == ShopifyCircuitState::half_open ||
        consecutive_failures_ >= failure_threshold_) {
        if (state_ != ShopifyCircuitState::open) ++openings_;
        state_ = ShopifyCircuitState::open;
        opened_at_ = std::chrono::steady_clock::now();
    }
}

ShopifyCircuitSnapshot ShopifyGraphqlCircuitBreaker::snapshot() const {
    std::lock_guard lock(mutex_);
    auto* self = const_cast<ShopifyGraphqlCircuitBreaker*>(this);
    self->refreshStateLocked(std::chrono::steady_clock::now());
    ShopifyCircuitSnapshot output;
    output.state = state_;
    output.failures = failures_;
    output.successes = successes_;
    output.rejected_requests = rejected_requests_;
    output.openings = openings_;
    output.consecutive_failures = consecutive_failures_;
    output.state_name = name(state_);
    return output;
}

} // namespace elit21::shopify::graphql
