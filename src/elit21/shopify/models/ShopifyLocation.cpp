#include "elit21/shopify/models/ShopifyLocation.h"

namespace elit21::shopify::models {

ShopifyLocation::ShopifyLocation()
    : platform::BusinessComponent(
          "ShopifyLocation",
          "Typed Shopify resource model",
          platform::BusinessComponentSpec{"shopify", "model_operation", {}, false, false, 4U * 1024U * 1024U}) {}

Result<void> ShopifyLocation::validate() const {
    if (id.empty()) return Result<void>::failure("ShopifyLocation: id is required");
    return Result<void>::success();
}

Json ShopifyLocation::toJson() const {
    Json json = Json::object();
    json.set("id", id);
    json.set("name", name);
    json.set("address1", address1);
    json.set("city", city);
    json.set("provinceCode", province_code);
    json.set("countryCode", country_code);
    json.set("active", active);
    return json;
}

Result<std::shared_ptr<ShopifyLocation>> ShopifyLocation::fromJson(const Json& json) {
    if (!json.isObject()) return Result<std::shared_ptr<ShopifyLocation>>::failure("ShopifyLocation: JSON object expected");
    auto value = std::make_shared<ShopifyLocation>();
    value->id = json.getScalarString("id", {});
    value->name = json.getScalarString("name", {});
    value->address1 = json.getScalarString("address1", {});
    value->city = json.getScalarString("city", {});
    value->province_code = json.getScalarString("provinceCode", {});
    value->country_code = json.getScalarString("countryCode", "CA");
    value->active = json.getBool("active", true);
    if (auto validation = value->validate(); !validation) return Result<std::shared_ptr<ShopifyLocation>>::failure(validation.error());
    return Result<std::shared_ptr<ShopifyLocation>>::success(std::move(value));
}

platform::OperationResult ShopifyLocation::execute(const platform::OperationContext& context) {
    auto result = executeBusiness(context);
    result.metrics["model_valid"] = valid() ? 1.0 : 0.0;
    return result;
}

} // namespace elit21::shopify::models
