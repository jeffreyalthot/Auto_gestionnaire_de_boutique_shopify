#include "elit21/shopify/ShopifyError.h"

namespace elit21::shopify {

std::string ShopifyErrorInfo::categoryName() const {
    switch (category) {
        case ShopifyErrorCategory::authentication: return "authentication";
        case ShopifyErrorCategory::authorization: return "authorization";
        case ShopifyErrorCategory::throttled: return "throttled";
        case ShopifyErrorCategory::validation: return "validation";
        case ShopifyErrorCategory::not_found: return "not_found";
        case ShopifyErrorCategory::conflict: return "conflict";
        case ShopifyErrorCategory::transient: return "transient";
        case ShopifyErrorCategory::transport: return "transport";
        default: return "unknown";
    }
}

Json ShopifyErrorInfo::toJson() const {
    Json output = Json::object();
    output.set("category", categoryName());
    output.set("code", code);
    output.set("message", message);
    output.set("http_status", static_cast<std::int64_t>(http_status));
    output.set("retryable", retryable);
    output.set("suggested_retry_seconds", suggested_retry_seconds);
    output.set("context", context);
    return output;
}

ShopifyError::ShopifyError()
    : platform::BusinessComponent(
          "ShopifyError", "Structured Shopify API error",
          platform::BusinessComponentSpec{"shopify", "classify_error", {}, false, false,
                                          4U * 1024U * 1024U}) {}

ShopifyError::ShopifyError(ShopifyErrorInfo information) : ShopifyError() {
    information_ = std::move(information);
}

platform::OperationResult ShopifyError::execute(const platform::OperationContext&) {
    auto result = platform::OperationResult::failure(
        information_.code.empty() ? "SHOPIFY_ERROR" : information_.code,
        information_.message.empty() ? "Shopify operation failed" : information_.message);
    result.attributes["category"] = information_.categoryName();
    result.attributes["retryable"] = information_.retryable ? "true" : "false";
    result.metrics["http_status"] = static_cast<double>(information_.http_status);
    result.metrics["suggested_retry_seconds"] = static_cast<double>(information_.suggested_retry_seconds);
    return result;
}

} // namespace elit21::shopify
