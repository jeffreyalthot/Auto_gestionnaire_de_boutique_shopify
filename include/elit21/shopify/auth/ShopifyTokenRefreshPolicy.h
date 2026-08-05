#pragma once

#include "elit21/shopify/ShopifyScopes.h"
#include "elit21/shopify/auth/ShopifyOAuthClient.h"

#include <string>

namespace elit21::shopify::auth {

enum class ShopifyTokenAction { none, retry, verify_scopes, reauthorize };

class ShopifyTokenRefreshPolicy {
public:
    static bool shouldReauthorize(bool token_present, bool unauthorized_response) {
        return !token_present || unauthorized_response;
    }
    static bool shouldReauthorize(const ShopifyAccessToken* token,
                                  long http_status,
                                  const ShopifyScopes& required_scopes);
    static bool isAuthenticationFailure(long http_status, const std::string& response_body);
    static ShopifyTokenAction action(const ShopifyAccessToken* token,
                                     long http_status,
                                     const std::string& response_body,
                                     const ShopifyScopes& required_scopes);
    static std::string actionName(ShopifyTokenAction action);
};

} // namespace elit21::shopify::auth
