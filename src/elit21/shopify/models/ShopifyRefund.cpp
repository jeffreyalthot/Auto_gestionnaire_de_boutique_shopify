#include "elit21/shopify/models/ShopifyRefund.h"

namespace elit21::shopify::models {

ShopifyRefund::ShopifyRefund()
    : platform::BusinessComponent(
          "ShopifyRefund",
          "Typed Shopify resource model",
          platform::BusinessComponentSpec{"shopify", "model_operation", {}, false, false, 4U * 1024U * 1024U}) {}

Result<void> ShopifyRefund::validate() const {
    if (id.empty()) return Result<void>::failure("ShopifyRefund: id is required");
    return Result<void>::success();
}

Json ShopifyRefund::toJson() const {
    Json json = Json::object();
    json.set("id", id);
    json.set("orderId", order_id);
    json.set("createdAt", created_at);
    json.set("note", note);
    json.set("total", total);
    return json;
}

Result<std::shared_ptr<ShopifyRefund>> ShopifyRefund::fromJson(const Json& json) {
    if (!json.isObject()) return Result<std::shared_ptr<ShopifyRefund>>::failure("ShopifyRefund: JSON object expected");
    auto value = std::make_shared<ShopifyRefund>();
    value->id = json.getScalarString("id", {});
    value->order_id = json.getScalarString("orderId", {});
    value->created_at = json.getScalarString("createdAt", {});
    value->note = json.getScalarString("note", {});
    value->total = json.getNumber("total", 0.0);
    if (auto validation = value->validate(); !validation) return Result<std::shared_ptr<ShopifyRefund>>::failure(validation.error());
    return Result<std::shared_ptr<ShopifyRefund>>::success(std::move(value));
}

platform::OperationResult ShopifyRefund::execute(const platform::OperationContext& context) {
    auto result = executeBusiness(context);
    result.metrics["model_valid"] = valid() ? 1.0 : 0.0;
    return result;
}

} // namespace elit21::shopify::models
