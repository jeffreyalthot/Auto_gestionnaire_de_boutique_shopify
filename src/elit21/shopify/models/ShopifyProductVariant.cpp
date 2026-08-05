#include "elit21/shopify/models/ShopifyProductVariant.h"

namespace elit21::shopify::models {

ShopifyProductVariant::ShopifyProductVariant()
    : platform::BusinessComponent(
          "ShopifyProductVariant",
          "Typed Shopify resource model",
          platform::BusinessComponentSpec{"shopify", "model_operation", {}, false, false, 4U * 1024U * 1024U}) {}

Result<void> ShopifyProductVariant::validate() const {
    if (id.empty()) return Result<void>::failure("ShopifyProductVariant: id is required");
    if (price < 0.0) return Result<void>::failure("Price cannot be negative");
    return Result<void>::success();
}

Json ShopifyProductVariant::toJson() const {
    Json json = Json::object();
    json.set("id", id);
    json.set("productId", product_id);
    json.set("title", title);
    json.set("sku", sku);
    json.set("price", price);
    json.set("inventoryQuantity", inventory_quantity);
    json.set("inventoryItemId", inventory_item_id);
    return json;
}

Result<std::shared_ptr<ShopifyProductVariant>> ShopifyProductVariant::fromJson(const Json& json) {
    if (!json.isObject()) return Result<std::shared_ptr<ShopifyProductVariant>>::failure("ShopifyProductVariant: JSON object expected");
    auto value = std::make_shared<ShopifyProductVariant>();
    value->id = json.getScalarString("id", {});
    value->product_id = json.getScalarString("productId", {});
    value->title = json.getScalarString("title", {});
    value->sku = json.getScalarString("sku", {});
    value->price = json.getNumber("price", 0.0);
    value->inventory_quantity = json.getInt("inventoryQuantity", 0);
    value->inventory_item_id = json.getScalarString("inventoryItemId", {});
    if (auto validation = value->validate(); !validation) return Result<std::shared_ptr<ShopifyProductVariant>>::failure(validation.error());
    return Result<std::shared_ptr<ShopifyProductVariant>>::success(std::move(value));
}

platform::OperationResult ShopifyProductVariant::execute(const platform::OperationContext& context) {
    auto result = executeBusiness(context);
    result.metrics["model_valid"] = valid() ? 1.0 : 0.0;
    return result;
}

} // namespace elit21::shopify::models
