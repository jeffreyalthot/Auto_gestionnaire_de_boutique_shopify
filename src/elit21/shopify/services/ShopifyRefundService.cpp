#include "elit21/shopify/services/ShopifyRefundService.h"

#include "elit21/shopify/graphql/ShopifyQueryBuilder.h"

#include <algorithm>

namespace elit21::shopify::services {

ShopifyRefundService::ShopifyRefundService()
    : platform::BusinessComponent("ShopifyRefundService", "Typed Shopify Admin GraphQL service",
          platform::BusinessComponentSpec{"shopify", "query_admin_resource", {}, false, true,
                                          4U * 1024U * 1024U}) {}

ShopifyRefundService::ShopifyRefundService(ShopifyClient& client) : ShopifyRefundService() { client_ = &client; }

ShopifyClient& ShopifyRefundService::requireClient() {
    if (!client_) throw std::logic_error("ShopifyRefundService is not bound to a ShopifyClient");
    return *client_;
}

Result<Json> ShopifyRefundService::executeDocument(const std::string& query, const Json& variables,
                                      const std::string& response_field) {
    if (query.empty()) return Result<Json>::failure("Shopify GraphQL document is empty");
    auto result = requireClient().graphql(query, variables);
    if (!result) return result;
    if (response_field.empty()) return result;
    const auto selected = result.value().get(response_field);
    if (selected.isNull()) return Result<Json>::failure("Shopify response field is absent: " + response_field);
    return Result<Json>::success(selected);
}

Result<Json> ShopifyRefundService::orderRefunds(int first, const std::string& after,
                              const std::string& search_or_id) {
    Json variables = shopify::graphql::ShopifyQueryBuilder::pageVariables(first, after);
    if (search_or_id.empty()) return Result<Json>::failure("ShopifyRefundService requires a Shopify GID");
    variables.set("id", search_or_id);
    return executeDocument(R"graphql(query Elit21Refunds($id:ID!){order(id:$id){id refunds{id createdAt note totalRefundedSet{shopMoney{amount currencyCode}} refundLineItems(first:100){nodes{quantity lineItem{id sku}}}}}})graphql", variables, "order");
}

platform::OperationResult ShopifyRefundService::execute(const platform::OperationContext& context) {
    auto result = executeBusiness(context);
    result.metrics["client_bound"] = bound() ? 1.0 : 0.0;
    result.attributes["service"] = "ShopifyRefundService";
    result.attributes["transport"] = "Shopify Admin GraphQL";
    return result;
}

} // namespace elit21::shopify::services
