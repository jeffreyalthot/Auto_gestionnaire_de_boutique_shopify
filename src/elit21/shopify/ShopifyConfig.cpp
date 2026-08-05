#include "elit21/shopify/ShopifyConfig.h"

#include "elit21/shopify/ShopifyEndpoints.h"

namespace elit21::shopify {

ShopifyConfig::ShopifyConfig()
    : platform::BusinessComponent(
          "ShopifyConfig", "Validated Shopify integration configuration",
          platform::BusinessComponentSpec{"shopify", "validate_configuration", {}, false, false,
                                          1024U * 1024U}) {}

ShopifyConfig::ShopifyConfig(::elit21::ShopifyConfig source, ShopifyScopes scopes)
    : ShopifyConfig() {
    source_ = std::move(source);
    scopes_ = std::move(scopes);
}

Result<void> ShopifyConfig::validate(bool require_token) const {
    if (!ShopifyEndpoints::validateShop(ShopifyEndpoints::normalizeShop(source_.shop))) {
        return Result<void>::failure("Shopify shop identifier is invalid");
    }
    auto version = ShopifyApiVersion::parse(source_.api_version);
    if (!version) return Result<void>::failure(version.error());
    if (require_token && source_.access_token.empty()) {
        return Result<void>::failure("Shopify Admin access token is missing");
    }
    if (!source_.webhook_base_url.empty() && source_.webhook_base_url.rfind("https://", 0) != 0) {
        return Result<void>::failure("Shopify webhook base URL must use HTTPS");
    }
    if (source_.webhook_port <= 0 || source_.webhook_port > 65535) {
        return Result<void>::failure("Shopify webhook port is outside the valid range");
    }
    if (scopes_.empty()) return Result<void>::failure("Shopify scope set is empty");
    return Result<void>::success();
}

std::string ShopifyConfig::adminEndpoint() const {
    auto version = ShopifyApiVersion::parse(source_.api_version);
    if (!version) return {};
    return ShopifyEndpoints::adminGraphql(ShopifyEndpoints::normalizeShop(source_.shop), version.value().value());
}

std::string ShopifyConfig::shopDomain() const {
    return ShopifyEndpoints::normalizeShop(source_.shop) + ".myshopify.com";
}

Json ShopifyConfig::sanitizedSummary() const {
    Json output = Json::object();
    output.set("shop", ShopifyEndpoints::normalizeShop(source_.shop));
    output.set("shop_domain", shopDomain());
    output.set("api_version", source_.api_version);
    output.set("graphql_endpoint", adminEndpoint());
    output.set("webhook_base_url", source_.webhook_base_url);
    output.set("webhook_port", source_.webhook_port);
    output.set("token_configured", !source_.access_token.empty());
    output.set("webhook_secret_configured", !source_.webhook_secret.empty());
    output.set("scopes", scopes_.commaSeparated());
    return output;
}

platform::OperationResult ShopifyConfig::execute(const platform::OperationContext& context) {
    auto valid = validate(!context.dry_run);
    if (!valid) return platform::OperationResult::failure("INVALID_SHOPIFY_CONFIG", valid.error());
    auto result = platform::OperationResult::ok("Shopify configuration validated");
    result.attributes["shop_domain"] = shopDomain();
    result.attributes["api_version"] = source_.api_version;
    result.attributes["mode"] = context.dry_run ? "dry_run" : "live";
    result.metrics["scope_count"] = static_cast<double>(scopes_.values().size());
    result.metrics["token_configured"] = source_.access_token.empty() ? 0.0 : 1.0;
    return result;
}

} // namespace elit21::shopify
