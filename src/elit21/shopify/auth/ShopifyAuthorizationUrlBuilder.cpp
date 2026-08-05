#include "elit21/shopify/auth/ShopifyAuthorizationUrlBuilder.h"

#include "elit21/shopify/ShopifyEndpoints.h"
#include "elit21/util/StringUtil.h"

namespace elit21::shopify::auth {

Result<void> ShopifyAuthorizationRequest::validate() const {
    if (!ShopifyEndpoints::validateShop(shop)) return Result<void>::failure("Invalid Shopify shop");
    if (client_id.empty()) return Result<void>::failure("Shopify client_id is empty");
    if (scopes.empty()) return Result<void>::failure("Shopify scopes are empty");
    if (!ShopifyEndpoints::isHttpsUrl(redirect_uri)) {
        return Result<void>::failure("Shopify redirect_uri must use HTTPS");
    }
    if (state.size() < 32) return Result<void>::failure("Shopify OAuth state is too short");
    return Result<void>::success();
}

Result<std::string> ShopifyAuthorizationUrlBuilder::build(const ShopifyAuthorizationRequest& request) {
    if (auto validation = request.validate(); !validation) {
        return Result<std::string>::failure(validation.error());
    }
    std::string url = ShopifyEndpoints::oauthAuthorize(request.shop) +
        "?client_id=" + util::urlEncode(request.client_id) +
        "&scope=" + util::urlEncode(request.scopes.commaSeparated()) +
        "&redirect_uri=" + util::urlEncode(request.redirect_uri) +
        "&state=" + util::urlEncode(request.state);
    if (request.grant_options_per_user) url += "&grant_options%5B%5D=per-user";
    if (request.force_auth) url += "&force_auth=true";
    return Result<std::string>::success(std::move(url));
}

std::string ShopifyAuthorizationUrlBuilder::build(const std::string& shop,
                                                   const std::string& client_id,
                                                   const ShopifyScopes& scopes,
                                                   const std::string& redirect_uri,
                                                   const std::string& state) {
    ShopifyAuthorizationRequest request{shop, client_id, scopes, redirect_uri, state};
    auto result = build(request);
    return result ? result.value() : std::string{};
}

} // namespace elit21::shopify::auth
