#pragma once

#include "elit21/core/Result.h"
#include "elit21/json/Json.h"
#include "elit21/platform/BusinessComponent.h"

#include <cstdint>
#include <memory>
#include <string>

namespace elit21::shopify::models {

class ShopifyPublication final : public platform::BusinessComponent {
public:
    ShopifyPublication();

    std::string id{};
    std::string name{};
    bool supports_future_publishing{false};

    [[nodiscard]] Result<void> validate() const;
    [[nodiscard]] bool valid() const { return static_cast<bool>(validate()); }
    [[nodiscard]] Json toJson() const;
    static Result<std::shared_ptr<ShopifyPublication>> fromJson(const Json& json);
    platform::OperationResult execute(const platform::OperationContext& context) override;
};

} // namespace elit21::shopify::models
