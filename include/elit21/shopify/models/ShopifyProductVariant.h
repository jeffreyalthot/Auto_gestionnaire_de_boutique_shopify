#pragma once

#include "elit21/core/Result.h"
#include "elit21/json/Json.h"
#include "elit21/platform/BusinessComponent.h"

#include <cstdint>
#include <memory>
#include <string>

namespace elit21::shopify::models {

class ShopifyProductVariant final : public platform::BusinessComponent {
public:
    ShopifyProductVariant();

    std::string id{};
    std::string product_id{};
    std::string title{};
    std::string sku{};
    double price{0};
    int inventory_quantity{0};
    std::string inventory_item_id{};

    [[nodiscard]] Result<void> validate() const;
    [[nodiscard]] bool valid() const { return static_cast<bool>(validate()); }
    [[nodiscard]] Json toJson() const;
    static Result<std::shared_ptr<ShopifyProductVariant>> fromJson(const Json& json);
    platform::OperationResult execute(const platform::OperationContext& context) override;
};

} // namespace elit21::shopify::models
