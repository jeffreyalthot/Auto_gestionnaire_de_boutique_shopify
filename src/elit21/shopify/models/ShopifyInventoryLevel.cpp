#include "elit21/shopify/models/ShopifyInventoryLevel.h"

namespace elit21::shopify::models {

ShopifyInventoryLevel::ShopifyInventoryLevel()
    : platform::BusinessComponent(
          "ShopifyInventoryLevel",
          "Typed Shopify resource model",
          platform::BusinessComponentSpec{"shopify", "model_operation", {}, false, false, 4U * 1024U * 1024U}) {}

Result<void> ShopifyInventoryLevel::validate() const {
    if (id.empty()) return Result<void>::failure("ShopifyInventoryLevel: id is required");
    return Result<void>::success();
}

Json ShopifyInventoryLevel::toJson() const {
    Json json = Json::object();
    json.set("id", id);
    json.set("inventoryItemId", inventory_item_id);
    json.set("locationId", location_id);
    json.set("available", available);
    return json;
}

Result<std::shared_ptr<ShopifyInventoryLevel>> ShopifyInventoryLevel::fromJson(const Json& json) {
    if (!json.isObject()) return Result<std::shared_ptr<ShopifyInventoryLevel>>::failure("ShopifyInventoryLevel: JSON object expected");
    auto value = std::make_shared<ShopifyInventoryLevel>();
    value->id = json.getScalarString("id", {});
    value->inventory_item_id = json.getScalarString("inventoryItemId", {});
    value->location_id = json.getScalarString("locationId", {});
    value->available = json.getInt("available", 0);
    if (auto validation = value->validate(); !validation) return Result<std::shared_ptr<ShopifyInventoryLevel>>::failure(validation.error());
    return Result<std::shared_ptr<ShopifyInventoryLevel>>::success(std::move(value));
}

platform::OperationResult ShopifyInventoryLevel::execute(const platform::OperationContext& context) {
    auto result = executeBusiness(context);
    result.metrics["model_valid"] = valid() ? 1.0 : 0.0;
    return result;
}

} // namespace elit21::shopify::models
