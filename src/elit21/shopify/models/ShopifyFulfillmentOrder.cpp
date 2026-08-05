#include "elit21/shopify/models/ShopifyFulfillmentOrder.h"

namespace elit21::shopify::models {

ShopifyFulfillmentOrder::ShopifyFulfillmentOrder()
    : platform::BusinessComponent(
          "ShopifyFulfillmentOrder",
          "Typed Shopify resource model",
          platform::BusinessComponentSpec{"shopify", "model_operation", {}, false, false, 4U * 1024U * 1024U}) {}

Result<void> ShopifyFulfillmentOrder::validate() const {
    if (id.empty()) return Result<void>::failure("ShopifyFulfillmentOrder: id is required");
    return Result<void>::success();
}

Json ShopifyFulfillmentOrder::toJson() const {
    Json json = Json::object();
    json.set("id", id);
    json.set("orderId", order_id);
    json.set("status", status);
    json.set("assignedLocationId", assigned_location_id);
    return json;
}

Result<std::shared_ptr<ShopifyFulfillmentOrder>> ShopifyFulfillmentOrder::fromJson(const Json& json) {
    if (!json.isObject()) return Result<std::shared_ptr<ShopifyFulfillmentOrder>>::failure("ShopifyFulfillmentOrder: JSON object expected");
    auto value = std::make_shared<ShopifyFulfillmentOrder>();
    value->id = json.getScalarString("id", {});
    value->order_id = json.getScalarString("orderId", {});
    value->status = json.getScalarString("status", {});
    value->assigned_location_id = json.getScalarString("assignedLocationId", {});
    if (auto validation = value->validate(); !validation) return Result<std::shared_ptr<ShopifyFulfillmentOrder>>::failure(validation.error());
    return Result<std::shared_ptr<ShopifyFulfillmentOrder>>::success(std::move(value));
}

platform::OperationResult ShopifyFulfillmentOrder::execute(const platform::OperationContext& context) {
    auto result = executeBusiness(context);
    result.metrics["model_valid"] = valid() ? 1.0 : 0.0;
    return result;
}

} // namespace elit21::shopify::models
