#pragma once

#include "elit21/core/Result.h"
#include "elit21/net/HttpClient.h"
#include "elit21/shopify/ShopifyScopes.h"

#include <chrono>
#include <string>

namespace elit21::shopify::auth {

struct ShopifyAccessToken {
    std::string access_token;
    std::string scope;
    std::string associated_user_id;
    bool associated_user_scope{false};
    std::chrono::system_clock::time_point issued_at{std::chrono::system_clock::now()};

    [[nodiscard]] bool valid() const { return access_token.size() >= 20; }
    [[nodiscard]] ShopifyScopes scopes() const { return ShopifyScopes::parse(scope); }
    [[nodiscard]] std::string masked() const;
};

class ShopifyOAuthClient {
public:
    explicit ShopifyOAuthClient(HttpClient& http) : http_(http) {}

    Result<ShopifyAccessToken> exchangeCode(const std::string& shop,
                                            const std::string& client_id,
                                            const std::string& client_secret,
                                            const std::string& code);
    Result<ShopifyAccessToken> exchangeSessionToken(const std::string& shop,
                                                    const std::string& client_id,
                                                    const std::string& client_secret,
                                                    const std::string& subject_token);

private:
    Result<ShopifyAccessToken> parseTokenResponse(const HttpResponse& response) const;
    HttpClient& http_;
};

} // namespace elit21::shopify::auth
