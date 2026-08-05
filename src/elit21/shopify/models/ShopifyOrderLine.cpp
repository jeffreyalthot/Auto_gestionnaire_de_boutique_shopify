#include "elit21/shopify/models/ShopifyOrderLine.h"

namespace elit21::shopify::models {

ShopifyOrderLine::ShopifyOrderLine()
    : platform::BusinessComponent(
          "ShopifyOrderLine",
          "Typed Shopify resource model",
          platform::BusinessComponentSpec{"shopify", "model_operation", {}, false, false, 4U * 1024U * 1024U}) {}

Result<void> ShopifyOrderLine::validate() const {
    if (id.empty()) return Result<void>::failure("ShopifyOrderLine: id is required");
    if (quantity <= 0) return Result<void>::failure("Quantity must be positive");
    return Result<void>::success();
}

Json ShopifyOrderLine::toJson() const {
    Json json = Json::object();
    json.set("id", id);
    json.set("title", title);
    json.set("sku", sku);
    json.set("quantity", quantity);
    json.set("variantId", variant_id);
    json.set("unitPrice", unit_price);
    return json;
}

Result<std::shared_ptr<ShopifyOrderLine>> ShopifyOrderLine::fromJson(const Json& json) {
    if (!json.isObject()) return Result<std::shared_ptr<ShopifyOrderLine>>::failure("ShopifyOrderLine: JSON object expected");
    auto value = std::make_shared<ShopifyOrderLine>();
    value->id = json.getScalarString("id", {});
    value->title = json.getScalarString("title", {});
    value->sku = json.getScalarString("sku", {});
    value->quantity = json.getInt("quantity", 0);
    value->variant_id = json.getScalarString("variantId", {});
    value->unit_price = json.getNumber("unitPrice", 0.0);
    if (auto validation = value->validate(); !validation) return Result<std::shared_ptr<ShopifyOrderLine>>::failure(validation.error());
    return Result<std::shared_ptr<ShopifyOrderLine>>::success(std::move(value));
}

platform::OperationResult ShopifyOrderLine::execute(const platform::OperationContext& context) {
    auto result = executeBusiness(context);
    result.metrics["model_valid"] = valid() ? 1.0 : 0.0;
    return result;
}

} // namespace elit21::shopify::models
