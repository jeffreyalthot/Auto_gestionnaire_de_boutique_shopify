#include "elit21/shopify/auth/ShopifyTokenRefreshPolicy.h"

#include "elit21/util/StringUtil.h"

namespace elit21::shopify::auth {

bool ShopifyTokenRefreshPolicy::isAuthenticationFailure(long http_status,
                                                         const std::string& response_body) {
    if (http_status == 401) return true;
    const auto lower = util::lower(response_body);
    return lower.find("invalid api key") != std::string::npos ||
           lower.find("invalid access token") != std::string::npos ||
           lower.find("token expired") != std::string::npos ||
           lower.find("session expired") != std::string::npos;
}

ShopifyTokenAction ShopifyTokenRefreshPolicy::action(const ShopifyAccessToken* token,
                                                      long http_status,
                                                      const std::string& response_body,
                                                      const ShopifyScopes& required_scopes) {
    if (!token || !token->valid()) return ShopifyTokenAction::reauthorize;
    if (isAuthenticationFailure(http_status, response_body)) return ShopifyTokenAction::reauthorize;
    if (!token->scopes().containsAll(required_scopes)) return ShopifyTokenAction::verify_scopes;
    if (http_status == 403 || util::lower(response_body).find("access denied") != std::string::npos)
        return ShopifyTokenAction::verify_scopes;
    if (http_status == 429 || http_status >= 500) return ShopifyTokenAction::retry;
    return ShopifyTokenAction::none;
}

bool ShopifyTokenRefreshPolicy::shouldReauthorize(const ShopifyAccessToken* token,
                                                   long http_status,
                                                   const ShopifyScopes& required_scopes) {
    return action(token, http_status, {}, required_scopes) == ShopifyTokenAction::reauthorize;
}

std::string ShopifyTokenRefreshPolicy::actionName(ShopifyTokenAction value) {
    switch (value) {
        case ShopifyTokenAction::retry: return "retry";
        case ShopifyTokenAction::verify_scopes: return "verify_scopes";
        case ShopifyTokenAction::reauthorize: return "reauthorize";
        default: return "none";
    }
}

} // namespace elit21::shopify::auth
