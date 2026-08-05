#include "elit21/shopify/models/ShopifyInventoryItem.h"

namespace elit21::shopify::models {

ShopifyInventoryItem::ShopifyInventoryItem()
    : platform::BusinessComponent(
          "ShopifyInventoryItem",
          "Typed Shopify resource model",
          platform::BusinessComponentSpec{"shopify", "model_operation", {}, false, false, 4U * 1024U * 1024U}) {}

Result<void> ShopifyInventoryItem::validate() const {
    if (id.empty()) return Result<void>::failure("ShopifyInventoryItem: id is required");
    return Result<void>::success();
}

Json ShopifyInventoryItem::toJson() const {
    Json json = Json::object();
    json.set("id", id);
    json.set("sku", sku);
    json.set("tracked", tracked);
    json.set("requiresShipping", requires_shipping);
    json.set("cost", cost);
    return json;
}

Result<std::shared_ptr<ShopifyInventoryItem>> ShopifyInventoryItem::fromJson(const Json& json) {
    if (!json.isObject()) return Result<std::shared_ptr<ShopifyInventoryItem>>::failure("ShopifyInventoryItem: JSON object expected");
    auto value = std::make_shared<ShopifyInventoryItem>();
    value->id = json.getScalarString("id", {});
    value->sku = json.getScalarString("sku", {});
    value->tracked = json.getBool("tracked", true);
    value->requires_shipping = json.getBool("requiresShipping", true);
    value->cost = json.getNumber("cost", 0.0);
    if (auto validation = value->validate(); !validation) return Result<std::shared_ptr<ShopifyInventoryItem>>::failure(validation.error());
    return Result<std::shared_ptr<ShopifyInventoryItem>>::success(std::move(value));
}

platform::OperationResult ShopifyInventoryItem::execute(const platform::OperationContext& context) {
    auto result = executeBusiness(context);
    result.metrics["model_valid"] = valid() ? 1.0 : 0.0;
    return result;
}

} // namespace elit21::shopify::models
