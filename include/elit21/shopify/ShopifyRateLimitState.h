#pragma once

#include "elit21/json/Json.h"
#include "elit21/platform/BusinessComponent.h"
#include "elit21/shopify/graphql/GraphqlTransport.h"

#include <chrono>
#include <cstdint>

namespace elit21::shopify {

class ShopifyRateLimitState final : public platform::BusinessComponent {
public:
    ShopifyRateLimitState();
    void update(const graphql::GraphqlTransportMetrics& metrics);
    void update(const Json& extensions);
    [[nodiscard]] double utilizationPercent() const noexcept;
    [[nodiscard]] bool healthy(double minimum_available = 100.0) const noexcept;
    [[nodiscard]] std::chrono::milliseconds delayFor(double requested_cost) const;
    [[nodiscard]] Json snapshot() const;
    platform::OperationResult execute(const platform::OperationContext& context) override;
private:
    std::uint64_t requests_{0};
    std::uint64_t retries_{0};
    std::uint64_t throttles_{0};
    double maximum_available_{1000.0};
    double currently_available_{1000.0};
    double restore_rate_{50.0};
};

} // namespace elit21::shopify
