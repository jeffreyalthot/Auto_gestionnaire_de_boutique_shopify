#include "elit21/shopify/models/ShopifyTrackingInfo.h"

namespace elit21::shopify::models {

ShopifyTrackingInfo::ShopifyTrackingInfo()
    : platform::BusinessComponent(
          "ShopifyTrackingInfo",
          "Typed Shopify resource model",
          platform::BusinessComponentSpec{"shopify", "model_operation", {}, false, false, 4U * 1024U * 1024U}) {}

Result<void> ShopifyTrackingInfo::validate() const {
    if (number.empty()) return Result<void>::failure("ShopifyTrackingInfo: number is required");
    return Result<void>::success();
}

Json ShopifyTrackingInfo::toJson() const {
    Json json = Json::object();
    json.set("company", company);
    json.set("number", number);
    json.set("url", url);
    return json;
}

Result<std::shared_ptr<ShopifyTrackingInfo>> ShopifyTrackingInfo::fromJson(const Json& json) {
    if (!json.isObject()) return Result<std::shared_ptr<ShopifyTrackingInfo>>::failure("ShopifyTrackingInfo: JSON object expected");
    auto value = std::make_shared<ShopifyTrackingInfo>();
    value->company = json.getScalarString("company", {});
    value->number = json.getScalarString("number", {});
    value->url = json.getScalarString("url", {});
    if (auto validation = value->validate(); !validation) return Result<std::shared_ptr<ShopifyTrackingInfo>>::failure(validation.error());
    return Result<std::shared_ptr<ShopifyTrackingInfo>>::success(std::move(value));
}

platform::OperationResult ShopifyTrackingInfo::execute(const platform::OperationContext& context) {
    auto result = executeBusiness(context);
    result.metrics["model_valid"] = valid() ? 1.0 : 0.0;
    return result;
}

} // namespace elit21::shopify::models
