#pragma once

#include "elit21/core/Result.h"
#include "elit21/json/Json.h"
#include "elit21/platform/BusinessComponent.h"

#include <cstdint>
#include <memory>
#include <string>

namespace elit21::shopify::models {

class ShopifyCustomer final : public platform::BusinessComponent {
public:
    ShopifyCustomer();

    std::string id{};
    std::string email{};
    std::string phone{};
    std::string first_name{};
    std::string last_name{};
    int orders_count{0};
    double total_spent{0};

    [[nodiscard]] Result<void> validate() const;
    [[nodiscard]] bool valid() const { return static_cast<bool>(validate()); }
    [[nodiscard]] Json toJson() const;
    static Result<std::shared_ptr<ShopifyCustomer>> fromJson(const Json& json);
    platform::OperationResult execute(const platform::OperationContext& context) override;
};

} // namespace elit21::shopify::models
