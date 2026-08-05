#include "elit21/shopify/models/ShopifyReturn.h"

namespace elit21::shopify::models {

ShopifyReturn::ShopifyReturn()
    : platform::BusinessComponent(
          "ShopifyReturn",
          "Typed Shopify resource model",
          platform::BusinessComponentSpec{"shopify", "model_operation", {}, false, false, 4U * 1024U * 1024U}) {}

Result<void> ShopifyReturn::validate() const {
    if (id.empty()) return Result<void>::failure("ShopifyReturn: id is required");
    return Result<void>::success();
}

Json ShopifyReturn::toJson() const {
    Json json = Json::object();
    json.set("id", id);
    json.set("orderId", order_id);
    json.set("status", status);
    json.set("name", name);
    return json;
}

Result<std::shared_ptr<ShopifyReturn>> ShopifyReturn::fromJson(const Json& json) {
    if (!json.isObject()) return Result<std::shared_ptr<ShopifyReturn>>::failure("ShopifyReturn: JSON object expected");
    auto value = std::make_shared<ShopifyReturn>();
    value->id = json.getScalarString("id", {});
    value->order_id = json.getScalarString("orderId", {});
    value->status = json.getScalarString("status", {});
    value->name = json.getScalarString("name", {});
    if (auto validation = value->validate(); !validation) return Result<std::shared_ptr<ShopifyReturn>>::failure(validation.error());
    return Result<std::shared_ptr<ShopifyReturn>>::success(std::move(value));
}

platform::OperationResult ShopifyReturn::execute(const platform::OperationContext& context) {
    auto result = executeBusiness(context);
    result.metrics["model_valid"] = valid() ? 1.0 : 0.0;
    return result;
}

} // namespace elit21::shopify::models
