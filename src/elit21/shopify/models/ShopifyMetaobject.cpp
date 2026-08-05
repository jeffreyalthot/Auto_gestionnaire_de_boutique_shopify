#include "elit21/shopify/models/ShopifyMetaobject.h"

namespace elit21::shopify::models {

ShopifyMetaobject::ShopifyMetaobject()
    : platform::BusinessComponent(
          "ShopifyMetaobject",
          "Typed Shopify resource model",
          platform::BusinessComponentSpec{"shopify", "model_operation", {}, false, false, 4U * 1024U * 1024U}) {}

Result<void> ShopifyMetaobject::validate() const {
    if (id.empty()) return Result<void>::failure("ShopifyMetaobject: id is required");
    return Result<void>::success();
}

Json ShopifyMetaobject::toJson() const {
    Json json = Json::object();
    json.set("id", id);
    json.set("type", type);
    json.set("handle", handle);
    json.set("updatedAt", updated_at);
    return json;
}

Result<std::shared_ptr<ShopifyMetaobject>> ShopifyMetaobject::fromJson(const Json& json) {
    if (!json.isObject()) return Result<std::shared_ptr<ShopifyMetaobject>>::failure("ShopifyMetaobject: JSON object expected");
    auto value = std::make_shared<ShopifyMetaobject>();
    value->id = json.getScalarString("id", {});
    value->type = json.getScalarString("type", {});
    value->handle = json.getScalarString("handle", {});
    value->updated_at = json.getScalarString("updatedAt", {});
    if (auto validation = value->validate(); !validation) return Result<std::shared_ptr<ShopifyMetaobject>>::failure(validation.error());
    return Result<std::shared_ptr<ShopifyMetaobject>>::success(std::move(value));
}

platform::OperationResult ShopifyMetaobject::execute(const platform::OperationContext& context) {
    auto result = executeBusiness(context);
    result.metrics["model_valid"] = valid() ? 1.0 : 0.0;
    return result;
}

} // namespace elit21::shopify::models
