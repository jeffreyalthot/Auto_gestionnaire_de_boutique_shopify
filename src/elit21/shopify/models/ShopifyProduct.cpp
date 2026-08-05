#include "elit21/shopify/models/ShopifyProduct.h"

namespace elit21::shopify::models {

ShopifyProduct::ShopifyProduct()
    : platform::BusinessComponent(
          "ShopifyProduct",
          "Typed Shopify resource model",
          platform::BusinessComponentSpec{"shopify", "model_operation", {}, false, false, 4U * 1024U * 1024U}) {}

Result<void> ShopifyProduct::validate() const {
    if (id.empty()) return Result<void>::failure("ShopifyProduct: id is required");
    return Result<void>::success();
}

Json ShopifyProduct::toJson() const {
    Json json = Json::object();
    json.set("id", id);
    json.set("title", title);
    json.set("handle", handle);
    json.set("status", status);
    json.set("vendor", vendor);
    json.set("productType", product_type);
    json.set("updatedAt", updated_at);
    return json;
}

Result<std::shared_ptr<ShopifyProduct>> ShopifyProduct::fromJson(const Json& json) {
    if (!json.isObject()) return Result<std::shared_ptr<ShopifyProduct>>::failure("ShopifyProduct: JSON object expected");
    auto value = std::make_shared<ShopifyProduct>();
    value->id = json.getScalarString("id", {});
    value->title = json.getScalarString("title", {});
    value->handle = json.getScalarString("handle", {});
    value->status = json.getScalarString("status", "DRAFT");
    value->vendor = json.getScalarString("vendor", {});
    value->product_type = json.getScalarString("productType", {});
    value->updated_at = json.getScalarString("updatedAt", {});
    if (auto validation = value->validate(); !validation) return Result<std::shared_ptr<ShopifyProduct>>::failure(validation.error());
    return Result<std::shared_ptr<ShopifyProduct>>::success(std::move(value));
}

platform::OperationResult ShopifyProduct::execute(const platform::OperationContext& context) {
    auto result = executeBusiness(context);
    result.metrics["model_valid"] = valid() ? 1.0 : 0.0;
    return result;
}

} // namespace elit21::shopify::models
