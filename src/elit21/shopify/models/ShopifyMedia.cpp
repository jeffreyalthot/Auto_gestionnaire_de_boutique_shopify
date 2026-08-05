#include "elit21/shopify/models/ShopifyMedia.h"

namespace elit21::shopify::models {

ShopifyMedia::ShopifyMedia()
    : platform::BusinessComponent(
          "ShopifyMedia",
          "Typed Shopify resource model",
          platform::BusinessComponentSpec{"shopify", "model_operation", {}, false, false, 4U * 1024U * 1024U}) {}

Result<void> ShopifyMedia::validate() const {
    if (id.empty()) return Result<void>::failure("ShopifyMedia: id is required");
    return Result<void>::success();
}

Json ShopifyMedia::toJson() const {
    Json json = Json::object();
    json.set("id", id);
    json.set("mediaContentType", media_content_type);
    json.set("status", status);
    json.set("alt", alt);
    json.set("sourceUrl", source_url);
    return json;
}

Result<std::shared_ptr<ShopifyMedia>> ShopifyMedia::fromJson(const Json& json) {
    if (!json.isObject()) return Result<std::shared_ptr<ShopifyMedia>>::failure("ShopifyMedia: JSON object expected");
    auto value = std::make_shared<ShopifyMedia>();
    value->id = json.getScalarString("id", {});
    value->media_content_type = json.getScalarString("mediaContentType", {});
    value->status = json.getScalarString("status", {});
    value->alt = json.getScalarString("alt", {});
    value->source_url = json.getScalarString("sourceUrl", {});
    if (auto validation = value->validate(); !validation) return Result<std::shared_ptr<ShopifyMedia>>::failure(validation.error());
    return Result<std::shared_ptr<ShopifyMedia>>::success(std::move(value));
}

platform::OperationResult ShopifyMedia::execute(const platform::OperationContext& context) {
    auto result = executeBusiness(context);
    result.metrics["model_valid"] = valid() ? 1.0 : 0.0;
    return result;
}

} // namespace elit21::shopify::models
