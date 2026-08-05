#include "elit21/shopify/models/ShopifyMarket.h"

namespace elit21::shopify::models {

ShopifyMarket::ShopifyMarket()
    : platform::BusinessComponent(
          "ShopifyMarket",
          "Typed Shopify resource model",
          platform::BusinessComponentSpec{"shopify", "model_operation", {}, false, false, 4U * 1024U * 1024U}) {}

Result<void> ShopifyMarket::validate() const {
    if (id.empty()) return Result<void>::failure("ShopifyMarket: id is required");
    return Result<void>::success();
}

Json ShopifyMarket::toJson() const {
    Json json = Json::object();
    json.set("id", id);
    json.set("name", name);
    json.set("status", status);
    json.set("primary", primary);
    return json;
}

Result<std::shared_ptr<ShopifyMarket>> ShopifyMarket::fromJson(const Json& json) {
    if (!json.isObject()) return Result<std::shared_ptr<ShopifyMarket>>::failure("ShopifyMarket: JSON object expected");
    auto value = std::make_shared<ShopifyMarket>();
    value->id = json.getScalarString("id", {});
    value->name = json.getScalarString("name", {});
    value->status = json.getScalarString("status", {});
    value->primary = json.getBool("primary", false);
    if (auto validation = value->validate(); !validation) return Result<std::shared_ptr<ShopifyMarket>>::failure(validation.error());
    return Result<std::shared_ptr<ShopifyMarket>>::success(std::move(value));
}

platform::OperationResult ShopifyMarket::execute(const platform::OperationContext& context) {
    auto result = executeBusiness(context);
    result.metrics["model_valid"] = valid() ? 1.0 : 0.0;
    return result;
}

} // namespace elit21::shopify::models
