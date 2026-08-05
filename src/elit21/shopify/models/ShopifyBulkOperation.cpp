#include "elit21/shopify/models/ShopifyBulkOperation.h"

namespace elit21::shopify::models {

ShopifyBulkOperation::ShopifyBulkOperation()
    : platform::BusinessComponent(
          "ShopifyBulkOperation",
          "Typed Shopify resource model",
          platform::BusinessComponentSpec{"shopify", "model_operation", {}, false, false, 4U * 1024U * 1024U}) {}

Result<void> ShopifyBulkOperation::validate() const {
    if (id.empty()) return Result<void>::failure("ShopifyBulkOperation: id is required");
    return Result<void>::success();
}

Json ShopifyBulkOperation::toJson() const {
    Json json = Json::object();
    json.set("id", id);
    json.set("type", type);
    json.set("status", status);
    json.set("url", url);
    json.set("partialDataUrl", partial_data_url);
    json.set("errorCode", error_code);
    json.set("objectCount", static_cast<std::int64_t>(object_count));
    return json;
}

Result<std::shared_ptr<ShopifyBulkOperation>> ShopifyBulkOperation::fromJson(const Json& json) {
    if (!json.isObject()) return Result<std::shared_ptr<ShopifyBulkOperation>>::failure("ShopifyBulkOperation: JSON object expected");
    auto value = std::make_shared<ShopifyBulkOperation>();
    value->id = json.getScalarString("id", {});
    value->type = json.getScalarString("type", {});
    value->status = json.getScalarString("status", {});
    value->url = json.getScalarString("url", {});
    value->partial_data_url = json.getScalarString("partialDataUrl", {});
    value->error_code = json.getScalarString("errorCode", {});
    value->object_count = json.getInt64("objectCount", 0);
    if (auto validation = value->validate(); !validation) return Result<std::shared_ptr<ShopifyBulkOperation>>::failure(validation.error());
    return Result<std::shared_ptr<ShopifyBulkOperation>>::success(std::move(value));
}

platform::OperationResult ShopifyBulkOperation::execute(const platform::OperationContext& context) {
    auto result = executeBusiness(context);
    result.metrics["model_valid"] = valid() ? 1.0 : 0.0;
    return result;
}

} // namespace elit21::shopify::models
