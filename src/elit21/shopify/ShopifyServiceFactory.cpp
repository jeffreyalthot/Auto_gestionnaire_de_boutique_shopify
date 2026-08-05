#include "elit21/shopify/ShopifyServiceFactory.h"

#include "elit21/shopify/ShopifyConfig.h"

namespace elit21::shopify {

ShopifyServiceFactory::ShopifyServiceFactory()
    : platform::BusinessComponent(
          "ShopifyServiceFactory", "Validated Shopify client and service factory",
          platform::BusinessComponentSpec{"shopify", "create_services", {}, true, true,
                                          1024U * 1024U}) {}

ShopifyServiceFactory::ShopifyServiceFactory(::elit21::ShopifyConfig config, HttpClient& http)
    : ShopifyServiceFactory() {
    bind(std::move(config), http);
}

void ShopifyServiceFactory::bind(::elit21::ShopifyConfig config, HttpClient& http) {
    config_ = std::move(config);
    http_ = &http;
}

bool ShopifyServiceFactory::ready() const noexcept {
    return http_ != nullptr && !config_.shop.empty() && !config_.access_token.empty();
}

Result<void> ShopifyServiceFactory::validate() const {
    if (!http_) return Result<void>::failure("ShopifyServiceFactory has no HttpClient");
    shopify::ShopifyConfig validated(config_);
    return validated.validate(true);
}

Result<std::unique_ptr<ShopifyClient>> ShopifyServiceFactory::createClient() const {
    auto valid = validate();
    if (!valid) return Result<std::unique_ptr<ShopifyClient>>::failure(valid.error());
    return Result<std::unique_ptr<ShopifyClient>>::success(
        std::make_unique<ShopifyClient>(config_, *http_));
}

platform::OperationResult ShopifyServiceFactory::execute(const platform::OperationContext& context) {
    auto valid = validate();
    if (!valid && !context.dry_run) {
        return platform::OperationResult::failure("SHOPIFY_FACTORY_NOT_READY", valid.error());
    }
    auto result = platform::OperationResult::ok("Shopify service factory inspected");
    result.metrics["http_bound"] = http_ ? 1.0 : 0.0;
    result.metrics["credentials_configured"] = config_.access_token.empty() ? 0.0 : 1.0;
    result.attributes["shop"] = config_.shop;
    result.attributes["api_version"] = config_.api_version;
    result.attributes["ready"] = ready() ? "true" : "false";
    return result;
}

} // namespace elit21::shopify
