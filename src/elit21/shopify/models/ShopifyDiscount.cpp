#include "elit21/shopify/models/ShopifyDiscount.h"

namespace elit21::shopify::models {

ShopifyDiscount::ShopifyDiscount()
    : platform::BusinessComponent(
          "ShopifyDiscount",
          "Typed Shopify resource model",
          platform::BusinessComponentSpec{"shopify", "model_operation", {}, false, false, 4U * 1024U * 1024U}) {}

Result<void> ShopifyDiscount::validate() const {
    if (id.empty()) return Result<void>::failure("ShopifyDiscount: id is required");
    return Result<void>::success();
}

Json ShopifyDiscount::toJson() const {
    Json json = Json::object();
    json.set("id", id);
    json.set("title", title);
    json.set("status", status);
    json.set("startsAt", starts_at);
    json.set("endsAt", ends_at);
    json.set("usageCount", usage_count);
    return json;
}

Result<std::shared_ptr<ShopifyDiscount>> ShopifyDiscount::fromJson(const Json& json) {
    if (!json.isObject()) return Result<std::shared_ptr<ShopifyDiscount>>::failure("ShopifyDiscount: JSON object expected");
    auto value = std::make_shared<ShopifyDiscount>();
    value->id = json.getScalarString("id", {});
    value->title = json.getScalarString("title", {});
    value->status = json.getScalarString("status", {});
    value->starts_at = json.getScalarString("startsAt", {});
    value->ends_at = json.getScalarString("endsAt", {});
    value->usage_count = json.getInt("usageCount", 0);
    if (auto validation = value->validate(); !validation) return Result<std::shared_ptr<ShopifyDiscount>>::failure(validation.error());
    return Result<std::shared_ptr<ShopifyDiscount>>::success(std::move(value));
}

platform::OperationResult ShopifyDiscount::execute(const platform::OperationContext& context) {
    auto result = executeBusiness(context);
    result.metrics["model_valid"] = valid() ? 1.0 : 0.0;
    return result;
}

} // namespace elit21::shopify::models
