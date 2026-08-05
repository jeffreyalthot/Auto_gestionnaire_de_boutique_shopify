#pragma once

#include "elit21/core/Result.h"
#include "elit21/shopify/ShopifyScopes.h"

#include <string>

namespace elit21::shopify::auth {

struct ShopifyAuthorizationRequest {
    std::string shop;
    std::string client_id;
    ShopifyScopes scopes;
    std::string redirect_uri;
    std::string state;
    bool grant_options_per_user{false};
    bool force_auth{false};

    [[nodiscard]] Result<void> validate() const;
};

class ShopifyAuthorizationUrlBuilder {
public:
    static Result<std::string> build(const ShopifyAuthorizationRequest& request);
    static std::string build(const std::string& shop,
                             const std::string& client_id,
                             const ShopifyScopes& scopes,
                             const std::string& redirect_uri,
                             const std::string& state);
};

} // namespace elit21::shopify::auth
