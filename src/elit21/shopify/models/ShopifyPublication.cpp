#include "elit21/shopify/models/ShopifyPublication.h"

namespace elit21::shopify::models {

ShopifyPublication::ShopifyPublication()
    : platform::BusinessComponent(
          "ShopifyPublication",
          "Typed Shopify resource model",
          platform::BusinessComponentSpec{"shopify", "model_operation", {}, false, false, 4U * 1024U * 1024U}) {}

Result<void> ShopifyPublication::validate() const {
    if (id.empty()) return Result<void>::failure("ShopifyPublication: id is required");
    return Result<void>::success();
}

Json ShopifyPublication::toJson() const {
    Json json = Json::object();
    json.set("id", id);
    json.set("name", name);
    json.set("supportsFuturePublishing", supports_future_publishing);
    return json;
}

Result<std::shared_ptr<ShopifyPublication>> ShopifyPublication::fromJson(const Json& json) {
    if (!json.isObject()) return Result<std::shared_ptr<ShopifyPublication>>::failure("ShopifyPublication: JSON object expected");
    auto value = std::make_shared<ShopifyPublication>();
    value->id = json.getScalarString("id", {});
    value->name = json.getScalarString("name", {});
    value->supports_future_publishing = json.getBool("supportsFuturePublishing", false);
    if (auto validation = value->validate(); !validation) return Result<std::shared_ptr<ShopifyPublication>>::failure(validation.error());
    return Result<std::shared_ptr<ShopifyPublication>>::success(std::move(value));
}

platform::OperationResult ShopifyPublication::execute(const platform::OperationContext& context) {
    auto result = executeBusiness(context);
    result.metrics["model_valid"] = valid() ? 1.0 : 0.0;
    return result;
}

} // namespace elit21::shopify::models
