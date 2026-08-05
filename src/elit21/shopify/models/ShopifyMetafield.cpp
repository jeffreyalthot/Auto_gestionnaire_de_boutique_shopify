#include "elit21/shopify/models/ShopifyMetafield.h"

namespace elit21::shopify::models {

ShopifyMetafield::ShopifyMetafield()
    : platform::BusinessComponent(
          "ShopifyMetafield",
          "Typed Shopify resource model",
          platform::BusinessComponentSpec{"shopify", "model_operation", {}, false, false, 4U * 1024U * 1024U}) {}

Result<void> ShopifyMetafield::validate() const {
    if (id.empty()) return Result<void>::failure("ShopifyMetafield: id is required");
    return Result<void>::success();
}

Json ShopifyMetafield::toJson() const {
    Json json = Json::object();
    json.set("id", id);
    json.set("namespace", namespace_name);
    json.set("key", key);
    json.set("type", type);
    json.set("value", value);
    json.set("ownerId", owner_id);
    return json;
}

Result<std::shared_ptr<ShopifyMetafield>> ShopifyMetafield::fromJson(const Json& json) {
    if (!json.isObject()) return Result<std::shared_ptr<ShopifyMetafield>>::failure("ShopifyMetafield: JSON object expected");
    auto value = std::make_shared<ShopifyMetafield>();
    value->id = json.getScalarString("id", {});
    value->namespace_name = json.getScalarString("namespace", {});
    value->key = json.getScalarString("key", {});
    value->type = json.getScalarString("type", {});
    value->value = json.getScalarString("value", {});
    value->owner_id = json.getScalarString("ownerId", {});
    if (auto validation = value->validate(); !validation) return Result<std::shared_ptr<ShopifyMetafield>>::failure(validation.error());
    return Result<std::shared_ptr<ShopifyMetafield>>::success(std::move(value));
}

platform::OperationResult ShopifyMetafield::execute(const platform::OperationContext& context) {
    auto result = executeBusiness(context);
    result.metrics["model_valid"] = valid() ? 1.0 : 0.0;
    return result;
}

} // namespace elit21::shopify::models
