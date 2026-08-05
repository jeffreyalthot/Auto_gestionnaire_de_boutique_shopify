#pragma once

#include "elit21/core/Result.h"
#include "elit21/shopify/auth/ShopifyOAuthStateStore.h"

#include <chrono>
#include <map>
#include <string>

namespace elit21::shopify::auth {

struct ShopifyOAuthCallback {
    std::string shop;
    std::string code;
    std::string state;
    std::string hmac;
    std::int64_t timestamp{0};
    std::map<std::string, std::string> parameters;
};

struct ShopifyOAuthCallbackVerification {
    ShopifyOAuthCallback callback;
    ShopifyOAuthState issued_state;
    std::string canonical_message;
};

class ShopifyOAuthCallbackVerifier {
public:
    explicit ShopifyOAuthCallbackVerifier(ShopifyOAuthStateStore& state_store)
        : state_store_(state_store) {}

    Result<ShopifyOAuthCallbackVerification> verify(
        const std::string& raw_query,
        const std::string& client_secret,
        std::chrono::seconds maximum_age = std::chrono::minutes(10));

    static Result<ShopifyOAuthCallback> parse(const std::string& raw_query);
    static std::string canonicalMessage(const std::map<std::string, std::string>& parameters);
    static std::string signature(const std::map<std::string, std::string>& parameters,
                                 const std::string& client_secret);

private:
    ShopifyOAuthStateStore& state_store_;
};

} // namespace elit21::shopify::auth
