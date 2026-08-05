#include "elit21/shopify/models/ShopifyOrder.h"

namespace elit21::shopify::models {

ShopifyOrder::ShopifyOrder()
    : platform::BusinessComponent(
          "ShopifyOrder",
          "Typed Shopify resource model",
          platform::BusinessComponentSpec{"shopify", "model_operation", {}, false, false, 4U * 1024U * 1024U}) {}

Result<void> ShopifyOrder::validate() const {
    if (id.empty()) return Result<void>::failure("ShopifyOrder: id is required");
    return Result<void>::success();
}

Json ShopifyOrder::toJson() const {
    Json json = Json::object();
    json.set("id", id);
    json.set("name", name);
    json.set("email", email);
    json.set("financialStatus", financial_status);
    json.set("fulfillmentStatus", fulfillment_status);
    json.set("total", total);
    json.set("currency", currency);
    return json;
}

Result<std::shared_ptr<ShopifyOrder>> ShopifyOrder::fromJson(const Json& json) {
    if (!json.isObject()) return Result<std::shared_ptr<ShopifyOrder>>::failure("ShopifyOrder: JSON object expected");
    auto value = std::make_shared<ShopifyOrder>();
    value->id = json.getScalarString("id", {});
    value->name = json.getScalarString("name", {});
    value->email = json.getScalarString("email", {});
    value->financial_status = json.getScalarString("financialStatus", {});
    value->fulfillment_status = json.getScalarString("fulfillmentStatus", {});
    value->total = json.getNumber("total", 0.0);
    value->currency = json.getScalarString("currency", "CAD");
    if (auto validation = value->validate(); !validation) return Result<std::shared_ptr<ShopifyOrder>>::failure(validation.error());
    return Result<std::shared_ptr<ShopifyOrder>>::success(std::move(value));
}

platform::OperationResult ShopifyOrder::execute(const platform::OperationContext& context) {
    auto result = executeBusiness(context);
    result.metrics["model_valid"] = valid() ? 1.0 : 0.0;
    return result;
}

} // namespace elit21::shopify::models
