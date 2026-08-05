#include "elit21/shopify/models/ShopifyCustomer.h"

namespace elit21::shopify::models {

ShopifyCustomer::ShopifyCustomer()
    : platform::BusinessComponent(
          "ShopifyCustomer",
          "Typed Shopify resource model",
          platform::BusinessComponentSpec{"shopify", "model_operation", {}, false, false, 4U * 1024U * 1024U}) {}

Result<void> ShopifyCustomer::validate() const {
    if (id.empty()) return Result<void>::failure("ShopifyCustomer: id is required");
    return Result<void>::success();
}

Json ShopifyCustomer::toJson() const {
    Json json = Json::object();
    json.set("id", id);
    json.set("email", email);
    json.set("phone", phone);
    json.set("firstName", first_name);
    json.set("lastName", last_name);
    json.set("ordersCount", orders_count);
    json.set("totalSpent", total_spent);
    return json;
}

Result<std::shared_ptr<ShopifyCustomer>> ShopifyCustomer::fromJson(const Json& json) {
    if (!json.isObject()) return Result<std::shared_ptr<ShopifyCustomer>>::failure("ShopifyCustomer: JSON object expected");
    auto value = std::make_shared<ShopifyCustomer>();
    value->id = json.getScalarString("id", {});
    value->email = json.getScalarString("email", {});
    value->phone = json.getScalarString("phone", {});
    value->first_name = json.getScalarString("firstName", {});
    value->last_name = json.getScalarString("lastName", {});
    value->orders_count = json.getInt("ordersCount", 0);
    value->total_spent = json.getNumber("totalSpent", 0.0);
    if (auto validation = value->validate(); !validation) return Result<std::shared_ptr<ShopifyCustomer>>::failure(validation.error());
    return Result<std::shared_ptr<ShopifyCustomer>>::success(std::move(value));
}

platform::OperationResult ShopifyCustomer::execute(const platform::OperationContext& context) {
    auto result = executeBusiness(context);
    result.metrics["model_valid"] = valid() ? 1.0 : 0.0;
    return result;
}

} // namespace elit21::shopify::models
