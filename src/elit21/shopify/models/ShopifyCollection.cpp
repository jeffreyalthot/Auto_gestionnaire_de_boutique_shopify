#include "elit21/shopify/models/ShopifyCollection.h"

namespace elit21::shopify::models {

ShopifyCollection::ShopifyCollection()
    : platform::BusinessComponent(
          "ShopifyCollection",
          "Typed Shopify resource model",
          platform::BusinessComponentSpec{"shopify", "model_operation", {}, false, false, 4U * 1024U * 1024U}) {}

Result<void> ShopifyCollection::validate() const {
    if (id.empty()) return Result<void>::failure("ShopifyCollection: id is required");
    return Result<void>::success();
}

Json ShopifyCollection::toJson() const {
    Json json = Json::object();
    json.set("id", id);
    json.set("title", title);
    json.set("handle", handle);
    json.set("productsCount", products_count);
    return json;
}

Result<std::shared_ptr<ShopifyCollection>> ShopifyCollection::fromJson(const Json& json) {
    if (!json.isObject()) return Result<std::shared_ptr<ShopifyCollection>>::failure("ShopifyCollection: JSON object expected");
    auto value = std::make_shared<ShopifyCollection>();
    value->id = json.getScalarString("id", {});
    value->title = json.getScalarString("title", {});
    value->handle = json.getScalarString("handle", {});
    value->products_count = json.getInt("productsCount", 0);
    if (auto validation = value->validate(); !validation) return Result<std::shared_ptr<ShopifyCollection>>::failure(validation.error());
    return Result<std::shared_ptr<ShopifyCollection>>::success(std::move(value));
}

platform::OperationResult ShopifyCollection::execute(const platform::OperationContext& context) {
    auto result = executeBusiness(context);
    result.metrics["model_valid"] = valid() ? 1.0 : 0.0;
    return result;
}

} // namespace elit21::shopify::models
