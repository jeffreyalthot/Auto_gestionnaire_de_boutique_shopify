#include "elit21/shopify/auth/ShopifyOAuthClient.h"

#include "elit21/json/Json.h"
#include "elit21/shopify/ShopifyEndpoints.h"

#include <map>

namespace elit21::shopify::auth {

std::string ShopifyAccessToken::masked() const {
    if (access_token.size() < 10) return "***";
    return access_token.substr(0, 5) + "..." + access_token.substr(access_token.size() - 4);
}

Result<ShopifyAccessToken> ShopifyOAuthClient::parseTokenResponse(const HttpResponse& response) const {
    if (response.status < 200 || response.status >= 300) {
        return Result<ShopifyAccessToken>::failure(
            "Shopify OAuth HTTP " + std::to_string(response.status) + ": " + response.body);
    }
    auto parsed = Json::parse(response.body);
    if (!parsed) return Result<ShopifyAccessToken>::failure(parsed.error());
    ShopifyAccessToken token;
    token.access_token = parsed.value().getString("access_token");
    token.scope = parsed.value().getString("scope");
    const auto user = parsed.value().get("associated_user");
    token.associated_user_id = user.getScalarString("id");
    token.associated_user_scope = !token.associated_user_id.empty();
    if (!token.valid()) {
        return Result<ShopifyAccessToken>::failure("Shopify OAuth response does not contain a valid access token");
    }
    return Result<ShopifyAccessToken>::success(std::move(token));
}

Result<ShopifyAccessToken> ShopifyOAuthClient::exchangeCode(const std::string& shop,
                                                            const std::string& client_id,
                                                            const std::string& client_secret,
                                                            const std::string& code) {
    if (!ShopifyEndpoints::validateShop(shop) || client_id.empty() || client_secret.empty() || code.empty()) {
        return Result<ShopifyAccessToken>::failure("Shopify OAuth parameters are incomplete");
    }
    const std::map<std::string, std::string> form{
        {"client_id", client_id}, {"client_secret", client_secret}, {"code", code}
    };
    auto response = http_.postForm(ShopifyEndpoints::oauthAccessToken(shop), form);
    if (!response) return Result<ShopifyAccessToken>::failure(response.error());
    return parseTokenResponse(response.value());
}

Result<ShopifyAccessToken> ShopifyOAuthClient::exchangeSessionToken(
    const std::string& shop,
    const std::string& client_id,
    const std::string& client_secret,
    const std::string& subject_token) {
    if (!ShopifyEndpoints::validateShop(shop) || client_id.empty() || client_secret.empty() || subject_token.empty()) {
        return Result<ShopifyAccessToken>::failure("Shopify token exchange parameters are incomplete");
    }
    const std::map<std::string, std::string> form{
        {"client_id", client_id},
        {"client_secret", client_secret},
        {"grant_type", "urn:ietf:params:oauth:grant-type:token-exchange"},
        {"subject_token", subject_token},
        {"subject_token_type", "urn:ietf:params:oauth:token-type:id_token"},
        {"requested_token_type", "urn:shopify:params:oauth:token-type:offline-access-token"}
    };
    auto response = http_.postForm(ShopifyEndpoints::oauthAccessToken(shop), form);
    if (!response) return Result<ShopifyAccessToken>::failure(response.error());
    return parseTokenResponse(response.value());
}

} // namespace elit21::shopify::auth
