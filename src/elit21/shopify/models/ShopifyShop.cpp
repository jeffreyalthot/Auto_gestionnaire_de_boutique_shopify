#include "elit21/shopify/models/ShopifyShop.h"

namespace elit21::shopify::models {

ShopifyShop::ShopifyShop()
    : platform::BusinessComponent(
          "ShopifyShop",
          "Typed Shopify resource model",
          platform::BusinessComponentSpec{"shopify", "model_operation", {}, false, false, 4U * 1024U * 1024U}) {}

Result<void> ShopifyShop::validate() const {
    if (id.empty()) return Result<void>::failure("ShopifyShop: id is required");
    return Result<void>::success();
}

Json ShopifyShop::toJson() const {
    Json json = Json::object();
    json.set("id", id);
    json.set("name", name);
    json.set("myshopifyDomain", myshopify_domain);
    json.set("currencyCode", currency_code);
    json.set("ianaTimezone", timezone);
    return json;
}

Result<std::shared_ptr<ShopifyShop>> ShopifyShop::fromJson(const Json& json) {
    if (!json.isObject()) return Result<std::shared_ptr<ShopifyShop>>::failure("ShopifyShop: JSON object expected");
    auto value = std::make_shared<ShopifyShop>();
    value->id = json.getScalarString("id", {});
    value->name = json.getScalarString("name", {});
    value->myshopify_domain = json.getScalarString("myshopifyDomain", {});
    value->currency_code = json.getScalarString("currencyCode", "CAD");
    value->timezone = json.getScalarString("ianaTimezone", {});
    if (auto validation = value->validate(); !validation) return Result<std::shared_ptr<ShopifyShop>>::failure(validation.error());
    return Result<std::shared_ptr<ShopifyShop>>::success(std::move(value));
}

platform::OperationResult ShopifyShop::execute(const platform::OperationContext& context) {
    auto result = executeBusiness(context);
    result.metrics["model_valid"] = valid() ? 1.0 : 0.0;
    return result;
}

} // namespace elit21::shopify::models
