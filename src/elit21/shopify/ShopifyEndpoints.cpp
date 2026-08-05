#include "elit21/shopify/ShopifyEndpoints.h"

#include "elit21/shopify/ShopifyApiVersion.h"
#include "elit21/util/StringUtil.h"

#include <algorithm>
#include <cctype>
#include <regex>

namespace elit21::shopify {

std::string ShopifyEndpoints::normalizeShop(std::string shop) {
    shop = util::lower(util::trim(std::move(shop)));
    constexpr const char* https = "https://";
    constexpr const char* http = "http://";
    if (shop.rfind(https, 0) == 0) shop.erase(0, std::char_traits<char>::length(https));
    if (shop.rfind(http, 0) == 0) shop.erase(0, std::char_traits<char>::length(http));
    const auto slash = shop.find('/');
    if (slash != std::string::npos) shop.resize(slash);
    constexpr const char* suffix = ".myshopify.com";
    if (shop.size() > std::char_traits<char>::length(suffix) &&
        shop.ends_with(suffix)) {
        shop.resize(shop.size() - std::char_traits<char>::length(suffix));
    }
    return shop;
}

Result<std::string> ShopifyEndpoints::validateShop(const std::string& shop) {
    const auto normalized = normalizeShop(shop);
    static const std::regex pattern(R"(^[a-z0-9][a-z0-9-]{1,61}[a-z0-9]$)");
    if (!std::regex_match(normalized, pattern)) {
        return Result<std::string>::failure("Invalid Shopify shop name: " + shop);
    }
    if (normalized.find("--") != std::string::npos) {
        return Result<std::string>::failure("Shopify shop name contains consecutive hyphens");
    }
    return Result<std::string>::success(normalized);
}

std::string ShopifyEndpoints::shopOrigin(const std::string& shop) {
    return "https://" + normalizeShop(shop) + ".myshopify.com";
}

std::string ShopifyEndpoints::adminGraphql(const std::string& shop, const std::string& version) {
    ShopifyApiVersion parsed(version);
    return shopOrigin(shop) + parsed.graphqlPath();
}

std::string ShopifyEndpoints::oauthAuthorize(const std::string& shop) {
    return shopOrigin(shop) + "/admin/oauth/authorize";
}

std::string ShopifyEndpoints::oauthAccessToken(const std::string& shop) {
    return shopOrigin(shop) + "/admin/oauth/access_token";
}

std::string ShopifyEndpoints::storefrontGraphql(const std::string& shop, const std::string& version) {
    ShopifyApiVersion parsed(version);
    return shopOrigin(shop) + parsed.storefrontPath();
}

std::string ShopifyEndpoints::customerAccountDiscovery(const std::string& shop_domain) {
    auto domain = util::trim(shop_domain);
    if (domain.rfind("https://", 0) != 0) domain = "https://" + domain;
    while (!domain.empty() && domain.back() == '/') domain.pop_back();
    return domain + "/.well-known/customer-account-api";
}

bool ShopifyEndpoints::isTrustedShopDomain(const std::string& domain) {
    auto normalized = util::lower(util::trim(domain));
    const auto colon = normalized.find(':');
    if (colon != std::string::npos) normalized.resize(colon);
    static const std::regex pattern(R"(^[a-z0-9][a-z0-9-]{1,61}[a-z0-9]\.myshopify\.com$)");
    return std::regex_match(normalized, pattern);
}

bool ShopifyEndpoints::isHttpsUrl(const std::string& url) {
    return util::lower(util::trim(url)).rfind("https://", 0) == 0;
}

} // namespace elit21::shopify
