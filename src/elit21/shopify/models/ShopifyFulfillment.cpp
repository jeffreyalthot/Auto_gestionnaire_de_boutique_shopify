#include "elit21/shopify/models/ShopifyFulfillment.h"

namespace elit21::shopify::models {

ShopifyFulfillment::ShopifyFulfillment()
    : platform::BusinessComponent(
          "ShopifyFulfillment",
          "Typed Shopify resource model",
          platform::BusinessComponentSpec{"shopify", "model_operation", {}, false, false, 4U * 1024U * 1024U}) {}

Result<void> ShopifyFulfillment::validate() const {
    if (id.empty()) return Result<void>::failure("ShopifyFulfillment: id is required");
    return Result<void>::success();
}

Json ShopifyFulfillment::toJson() const {
    Json json = Json::object();
    json.set("id", id);
    json.set("orderId", order_id);
    json.set("status", status);
    json.set("trackingCompany", tracking_company);
    json.set("trackingNumber", tracking_number);
    return json;
}

Result<std::shared_ptr<ShopifyFulfillment>> ShopifyFulfillment::fromJson(const Json& json) {
    if (!json.isObject()) return Result<std::shared_ptr<ShopifyFulfillment>>::failure("ShopifyFulfillment: JSON object expected");
    auto value = std::make_shared<ShopifyFulfillment>();
    value->id = json.getScalarString("id", {});
    value->order_id = json.getScalarString("orderId", {});
    value->status = json.getScalarString("status", {});
    value->tracking_company = json.getScalarString("trackingCompany", {});
    value->tracking_number = json.getScalarString("trackingNumber", {});
    if (auto validation = value->validate(); !validation) return Result<std::shared_ptr<ShopifyFulfillment>>::failure(validation.error());
    return Result<std::shared_ptr<ShopifyFulfillment>>::success(std::move(value));
}

platform::OperationResult ShopifyFulfillment::execute(const platform::OperationContext& context) {
    auto result = executeBusiness(context);
    result.metrics["model_valid"] = valid() ? 1.0 : 0.0;
    return result;
}

} // namespace elit21::shopify::models
