#include "elit21/shopify/models/ShopifyTransaction.h"

namespace elit21::shopify::models {

ShopifyTransaction::ShopifyTransaction()
    : platform::BusinessComponent(
          "ShopifyTransaction",
          "Typed Shopify resource model",
          platform::BusinessComponentSpec{"shopify", "model_operation", {}, false, false, 4U * 1024U * 1024U}) {}

Result<void> ShopifyTransaction::validate() const {
    if (id.empty()) return Result<void>::failure("ShopifyTransaction: id is required");
    if (amount < 0.0) return Result<void>::failure("Amount cannot be negative");
    return Result<void>::success();
}

Json ShopifyTransaction::toJson() const {
    Json json = Json::object();
    json.set("id", id);
    json.set("orderId", order_id);
    json.set("kind", kind);
    json.set("status", status);
    json.set("gateway", gateway);
    json.set("amount", amount);
    json.set("currency", currency);
    return json;
}

Result<std::shared_ptr<ShopifyTransaction>> ShopifyTransaction::fromJson(const Json& json) {
    if (!json.isObject()) return Result<std::shared_ptr<ShopifyTransaction>>::failure("ShopifyTransaction: JSON object expected");
    auto value = std::make_shared<ShopifyTransaction>();
    value->id = json.getScalarString("id", {});
    value->order_id = json.getScalarString("orderId", {});
    value->kind = json.getScalarString("kind", {});
    value->status = json.getScalarString("status", {});
    value->gateway = json.getScalarString("gateway", {});
    value->amount = json.getNumber("amount", 0.0);
    value->currency = json.getScalarString("currency", "CAD");
    if (auto validation = value->validate(); !validation) return Result<std::shared_ptr<ShopifyTransaction>>::failure(validation.error());
    return Result<std::shared_ptr<ShopifyTransaction>>::success(std::move(value));
}

platform::OperationResult ShopifyTransaction::execute(const platform::OperationContext& context) {
    auto result = executeBusiness(context);
    result.metrics["model_valid"] = valid() ? 1.0 : 0.0;
    return result;
}

} // namespace elit21::shopify::models
