#pragma once

#include "elit21/core/Result.h"
#include "elit21/json/Json.h"
#include "elit21/platform/BusinessComponent.h"

#include <cstdint>
#include <memory>
#include <string>

namespace elit21::shopify::models {

class ShopifyOrder final : public platform::BusinessComponent {
public:
    ShopifyOrder();

    std::string id{};
    std::string name{};
    std::string email{};
    std::string financial_status{};
    std::string fulfillment_status{};
    double total{0};
    std::string currency{"CAD"};

    [[nodiscard]] Result<void> validate() const;
    [[nodiscard]] bool valid() const { return static_cast<bool>(validate()); }
    [[nodiscard]] Json toJson() const;
    static Result<std::shared_ptr<ShopifyOrder>> fromJson(const Json& json);
    platform::OperationResult execute(const platform::OperationContext& context) override;
};

} // namespace elit21::shopify::models
