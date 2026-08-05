#include "elit21/shopify/services/ShopifyDraftOrderService.h"

#include "elit21/shopify/graphql/ShopifyQueryBuilder.h"

#include <algorithm>

namespace elit21::shopify::services {

ShopifyDraftOrderService::ShopifyDraftOrderService()
    : platform::BusinessComponent("ShopifyDraftOrderService", "Typed Shopify Admin GraphQL service",
          platform::BusinessComponentSpec{"shopify", "query_admin_resource", {}, false, true,
                                          4U * 1024U * 1024U}) {}

ShopifyDraftOrderService::ShopifyDraftOrderService(ShopifyClient& client) : ShopifyDraftOrderService() { client_ = &client; }

ShopifyClient& ShopifyDraftOrderService::requireClient() {
    if (!client_) throw std::logic_error("ShopifyDraftOrderService is not bound to a ShopifyClient");
    return *client_;
}

Result<Json> ShopifyDraftOrderService::executeDocument(const std::string& query, const Json& variables,
                                      const std::string& response_field) {
    if (query.empty()) return Result<Json>::failure("Shopify GraphQL document is empty");
    auto result = requireClient().graphql(query, variables);
    if (!result) return result;
    if (response_field.empty()) return result;
    const auto selected = result.value().get(response_field);
    if (selected.isNull()) return Result<Json>::failure("Shopify response field is absent: " + response_field);
    return Result<Json>::success(selected);
}

Result<Json> ShopifyDraftOrderService::draftOrdersPage(int first, const std::string& after,
                              const std::string& search_or_id) {
    Json variables = shopify::graphql::ShopifyQueryBuilder::pageVariables(first, after);
    if (!search_or_id.empty()) variables.set("query", search_or_id);
    return executeDocument(R"graphql(query Elit21DraftOrders($first:Int!,$after:String,$query:String){draftOrders(first:$first,after:$after,query:$query,sortKey:UPDATED_AT){nodes{id name status totalPriceSet{shopMoney{amount currencyCode}}updatedAt}pageInfo{hasNextPage endCursor}}})graphql", variables, "draftOrders");
}

platform::OperationResult ShopifyDraftOrderService::execute(const platform::OperationContext& context) {
    auto result = executeBusiness(context);
    result.metrics["client_bound"] = bound() ? 1.0 : 0.0;
    result.attributes["service"] = "ShopifyDraftOrderService";
    result.attributes["transport"] = "Shopify Admin GraphQL";
    return result;
}

} // namespace elit21::shopify::services
