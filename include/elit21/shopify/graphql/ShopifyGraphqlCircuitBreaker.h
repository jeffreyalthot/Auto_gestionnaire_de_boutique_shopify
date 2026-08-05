#pragma once

#include <chrono>
#include <cstdint>
#include <mutex>
#include <string>

namespace elit21::shopify::graphql {

enum class ShopifyCircuitState { closed, open, half_open };

struct ShopifyCircuitSnapshot {
    ShopifyCircuitState state{ShopifyCircuitState::closed};
    std::uint64_t failures{0};
    std::uint64_t successes{0};
    std::uint64_t rejected_requests{0};
    std::uint64_t openings{0};
    int consecutive_failures{0};
    std::string state_name{"closed"};
};

class ShopifyGraphqlCircuitBreaker {
public:
    explicit ShopifyGraphqlCircuitBreaker(
        int failure_threshold = 5,
        std::chrono::milliseconds open_duration = std::chrono::seconds(30));

    [[nodiscard]] bool allowRequest();
    void recordSuccess();
    void recordFailure();
    [[nodiscard]] ShopifyCircuitSnapshot snapshot() const;

private:
    void refreshStateLocked(std::chrono::steady_clock::time_point now);
    static std::string name(ShopifyCircuitState state);

    int failure_threshold_{5};
    std::chrono::milliseconds open_duration_{30000};
    mutable std::mutex mutex_;
    ShopifyCircuitState state_{ShopifyCircuitState::closed};
    std::chrono::steady_clock::time_point opened_at_{};
    int consecutive_failures_{0};
    std::uint64_t failures_{0};
    std::uint64_t successes_{0};
    std::uint64_t rejected_requests_{0};
    std::uint64_t openings_{0};
    bool half_open_probe_in_flight_{false};
};

} // namespace elit21::shopify::graphql
