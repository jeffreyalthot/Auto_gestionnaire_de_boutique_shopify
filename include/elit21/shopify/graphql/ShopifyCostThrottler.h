#pragma once

#include "elit21/json/Json.h"

#include <chrono>
#include <mutex>

namespace elit21::shopify::graphql {

struct ShopifyCostSnapshot {
    double maximum_available{1000.0};
    double currently_available{1000.0};
    double restore_rate{50.0};
    double last_requested_cost{0.0};
    double last_actual_cost{0.0};
};

class ShopifyCostThrottler {
public:
    void update(const Json& extensions);
    void reserve(double estimated_cost);
    [[nodiscard]] std::chrono::milliseconds recommendedDelay(double requested_cost = 1.0) const;
    [[nodiscard]] double available() const;
    [[nodiscard]] ShopifyCostSnapshot snapshot() const;
    void reset(double available = 1000.0, double restore_rate = 50.0);

private:
    mutable std::mutex mutex_;
    double maximum_available_{1000.0};
    double available_{1000.0};
    double restore_rate_{50.0};
    double requested_cost_{0.0};
    double actual_cost_{0.0};
    std::chrono::steady_clock::time_point updated_at_{std::chrono::steady_clock::now()};
};

} // namespace elit21::shopify::graphql
