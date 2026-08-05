#pragma once

#include "elit21/core/Result.h"
#include "elit21/json/Json.h"
#include "elit21/platform/BusinessComponent.h"

#include <cstdint>
#include <memory>
#include <string>

namespace elit21::shopify::models {

class ShopifyBulkOperation final : public platform::BusinessComponent {
public:
    ShopifyBulkOperation();

    std::string id{};
    std::string type{};
    std::string status{};
    std::string url{};
    std::string partial_data_url{};
    std::string error_code{};
    long long object_count{0};

    [[nodiscard]] Result<void> validate() const;
    [[nodiscard]] bool valid() const { return static_cast<bool>(validate()); }
    [[nodiscard]] Json toJson() const;
    static Result<std::shared_ptr<ShopifyBulkOperation>> fromJson(const Json& json);
    platform::OperationResult execute(const platform::OperationContext& context) override;
};

} // namespace elit21::shopify::models
