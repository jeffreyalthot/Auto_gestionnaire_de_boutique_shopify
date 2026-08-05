#pragma once

#include "elit21/config/Config.h"
#include "elit21/core/Result.h"
#include "elit21/json/Json.h"
#include "elit21/platform/BusinessComponent.h"
#include "elit21/shopify/ShopifyApiVersion.h"
#include "elit21/shopify/ShopifyScopes.h"

#include <string>

namespace elit21::shopify {

class ShopifyConfig final : public platform::BusinessComponent {
public:
    ShopifyConfig();
    explicit ShopifyConfig(::elit21::ShopifyConfig source,
                           ShopifyScopes scopes = ShopifyScopes::autonomousManagerDefaults());

    [[nodiscard]] const ::elit21::ShopifyConfig& source() const noexcept { return source_; }
    [[nodiscard]] const ShopifyScopes& scopes() const noexcept { return scopes_; }
    [[nodiscard]] Result<void> validate(bool require_token = true) const;
    [[nodiscard]] std::string adminEndpoint() const;
    [[nodiscard]] std::string shopDomain() const;
    [[nodiscard]] Json sanitizedSummary() const;
    platform::OperationResult execute(const platform::OperationContext& context) override;

private:
    ::elit21::ShopifyConfig source_;
    ShopifyScopes scopes_;
};

} // namespace elit21::shopify
