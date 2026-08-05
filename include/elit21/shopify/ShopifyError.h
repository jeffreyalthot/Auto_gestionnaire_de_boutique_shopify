#pragma once

#include "elit21/json/Json.h"
#include "elit21/platform/BusinessComponent.h"

#include <string>

namespace elit21::shopify {

enum class ShopifyErrorCategory {
    authentication,
    authorization,
    throttled,
    validation,
    not_found,
    conflict,
    transient,
    transport,
    unknown
};

struct ShopifyErrorInfo {
    ShopifyErrorCategory category{ShopifyErrorCategory::unknown};
    std::string code;
    std::string message;
    long http_status{0};
    bool retryable{false};
    int suggested_retry_seconds{0};
    Json context{Json::object()};

    [[nodiscard]] std::string categoryName() const;
    [[nodiscard]] Json toJson() const;
};

class ShopifyError final : public platform::BusinessComponent {
public:
    ShopifyError();
    explicit ShopifyError(ShopifyErrorInfo information);
    [[nodiscard]] const ShopifyErrorInfo& information() const noexcept { return information_; }
    [[nodiscard]] bool retryable() const noexcept { return information_.retryable; }
    platform::OperationResult execute(const platform::OperationContext& context) override;
private:
    ShopifyErrorInfo information_;
};

} // namespace elit21::shopify
