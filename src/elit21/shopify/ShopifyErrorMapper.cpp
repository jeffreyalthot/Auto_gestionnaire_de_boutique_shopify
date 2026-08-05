#include "elit21/shopify/ShopifyErrorMapper.h"

#include "elit21/util/StringUtil.h"

namespace elit21::shopify {

ShopifyErrorMapper::ShopifyErrorMapper()
    : platform::BusinessComponent(
          "ShopifyErrorMapper", "Shopify HTTP and GraphQL error classifier",
          platform::BusinessComponentSpec{"shopify", "map_error", {}, false, false,
                                          4U * 1024U * 1024U}) {}

ShopifyErrorInfo ShopifyErrorMapper::fromHttp(long status,
                                               const std::string& body,
                                               int retry_after_seconds) {
    ShopifyErrorInfo error;
    error.http_status = status;
    error.message = body.empty() ? "Shopify HTTP " + std::to_string(status) : body;
    error.suggested_retry_seconds = retry_after_seconds;
    if (status == 401) {
        error.category = ShopifyErrorCategory::authentication;
        error.code = "SHOPIFY_UNAUTHENTICATED";
    } else if (status == 403) {
        error.category = ShopifyErrorCategory::authorization;
        error.code = "SHOPIFY_FORBIDDEN";
    } else if (status == 404) {
        error.category = ShopifyErrorCategory::not_found;
        error.code = "SHOPIFY_NOT_FOUND";
    } else if (status == 409) {
        error.category = ShopifyErrorCategory::conflict;
        error.code = "SHOPIFY_CONFLICT";
    } else if (status == 422 || status == 400) {
        error.category = ShopifyErrorCategory::validation;
        error.code = "SHOPIFY_VALIDATION";
    } else if (status == 429) {
        error.category = ShopifyErrorCategory::throttled;
        error.code = "SHOPIFY_THROTTLED";
        error.retryable = true;
    } else if (status >= 500) {
        error.category = ShopifyErrorCategory::transient;
        error.code = "SHOPIFY_SERVER_ERROR";
        error.retryable = true;
    } else {
        error.category = ShopifyErrorCategory::unknown;
        error.code = "SHOPIFY_HTTP_ERROR";
    }
    return error;
}

ShopifyErrorInfo ShopifyErrorMapper::fromTransport(const std::string& message) {
    ShopifyErrorInfo error;
    error.category = ShopifyErrorCategory::transport;
    error.code = "SHOPIFY_TRANSPORT_ERROR";
    error.message = message;
    error.retryable = true;
    return error;
}

ShopifyErrorInfo ShopifyErrorMapper::fromGraphql(const graphql::GraphqlResponse& response,
                                                  long http_status) {
    ShopifyErrorInfo error;
    error.http_status = http_status;
    error.message = response.combinedErrorMessage();
    error.context = response.extensions();
    error.retryable = response.retryable();
    if (response.throttled()) {
        error.category = ShopifyErrorCategory::throttled;
        error.code = "SHOPIFY_GRAPHQL_THROTTLED";
        error.retryable = true;
    } else if (response.retryable()) {
        error.category = ShopifyErrorCategory::transient;
        error.code = "SHOPIFY_GRAPHQL_TRANSIENT";
    } else {
        error.category = ShopifyErrorCategory::validation;
        error.code = "SHOPIFY_GRAPHQL_ERROR";
    }
    return error;
}

platform::OperationResult ShopifyErrorMapper::execute(const platform::OperationContext& context) {
    auto parsed = Json::parse(context.payload);
    if (!parsed) return platform::OperationResult::failure("INVALID_ERROR_PAYLOAD", parsed.error());
    auto mapped = fromHttp(parsed.value().getInt("http_status", 500),
                           parsed.value().getString("message"),
                           parsed.value().getInt("retry_after_seconds", 0));
    auto result = platform::OperationResult::ok("Shopify error classified");
    result.attributes["category"] = mapped.categoryName();
    result.attributes["code"] = mapped.code;
    result.attributes["retryable"] = mapped.retryable ? "true" : "false";
    result.metrics["http_status"] = static_cast<double>(mapped.http_status);
    return result;
}

} // namespace elit21::shopify
