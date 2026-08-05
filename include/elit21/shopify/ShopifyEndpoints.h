#pragma once

#include "elit21/core/Result.h"

#include <string>

namespace elit21::shopify {

class ShopifyEndpoints {
public:
    static std::string normalizeShop(std::string shop);
    static Result<std::string> validateShop(const std::string& shop);
    static std::string shopOrigin(const std::string& shop);
    static std::string adminGraphql(const std::string& shop,
                                    const std::string& api_version = "2026-07");
    static std::string oauthAuthorize(const std::string& shop);
    static std::string oauthAccessToken(const std::string& shop);
    static std::string storefrontGraphql(const std::string& shop,
                                         const std::string& api_version = "2026-07");
    static std::string customerAccountDiscovery(const std::string& shop_domain);
    static bool isTrustedShopDomain(const std::string& domain);
    static bool isHttpsUrl(const std::string& url);
};

} // namespace elit21::shopify
