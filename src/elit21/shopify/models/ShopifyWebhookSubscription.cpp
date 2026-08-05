#include "elit21/shopify/models/ShopifyWebhookSubscription.h"

namespace elit21::shopify::models {

ShopifyWebhookSubscription::ShopifyWebhookSubscription()
    : platform::BusinessComponent(
          "ShopifyWebhookSubscription",
          "Typed Shopify resource model",
          platform::BusinessComponentSpec{"shopify", "model_operation", {}, false, false, 4U * 1024U * 1024U}) {}

Result<void> ShopifyWebhookSubscription::validate() const {
    if (id.empty()) return Result<void>::failure("ShopifyWebhookSubscription: id is required");
    return Result<void>::success();
}

Json ShopifyWebhookSubscription::toJson() const {
    Json json = Json::object();
    json.set("id", id);
    json.set("topic", topic);
    json.set("callbackUrl", callback_url);
    json.set("format", format);
    json.set("createdAt", created_at);
    return json;
}

Result<std::shared_ptr<ShopifyWebhookSubscription>> ShopifyWebhookSubscription::fromJson(const Json& json) {
    if (!json.isObject()) return Result<std::shared_ptr<ShopifyWebhookSubscription>>::failure("ShopifyWebhookSubscription: JSON object expected");
    auto value = std::make_shared<ShopifyWebhookSubscription>();
    value->id = json.getScalarString("id", {});
    value->topic = json.getScalarString("topic", {});
    value->callback_url = json.getScalarString("callbackUrl", {});
    value->format = json.getScalarString("format", "JSON");
    value->created_at = json.getScalarString("createdAt", {});
    if (auto validation = value->validate(); !validation) return Result<std::shared_ptr<ShopifyWebhookSubscription>>::failure(validation.error());
    return Result<std::shared_ptr<ShopifyWebhookSubscription>>::success(std::move(value));
}

platform::OperationResult ShopifyWebhookSubscription::execute(const platform::OperationContext& context) {
    auto result = executeBusiness(context);
    result.metrics["model_valid"] = valid() ? 1.0 : 0.0;
    return result;
}

} // namespace elit21::shopify::models
