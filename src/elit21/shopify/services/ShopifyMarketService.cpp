#include "elit21/shopify/services/ShopifyMarketService.h"

#include "elit21/shopify/graphql/ShopifyQueryBuilder.h"

#include <algorithm>

namespace elit21::shopify::services {

ShopifyMarketService::ShopifyMarketService()
    : platform::BusinessComponent("ShopifyMarketService", "Typed Shopify Admin GraphQL service",
          platform::BusinessComponentSpec{"shopify", "query_admin_resource", {}, false, true,
                                          4U * 1024U * 1024U}) {}

ShopifyMarketService::ShopifyMarketService(ShopifyClient& client) : ShopifyMarketService() { client_ = &client; }

ShopifyClient& ShopifyMarketService::requireClient() {
    if (!client_) throw std::logic_error("ShopifyMarketService is not bound to a ShopifyClient");
    return *client_;
}

Result<Json> ShopifyMarketService::executeDocument(const std::string& query, const Json& variables,
                                      const std::string& response_field) {
    if (query.empty()) return Result<Json>::failure("Shopify GraphQL document is empty");
    auto result = requireClient().graphql(query, variables);
    if (!result) return result;
    if (response_field.empty()) return result;
    const auto selected = result.value().get(response_field);
    if (selected.isNull()) return Result<Json>::failure("Shopify response field is absent: " + response_field);
    return Result<Json>::success(selected);
}

Result<Json> ShopifyMarketService::marketsPage(int first, const std::string& after,
                              const std::string& search_or_id) {
    Json variables = shopify::graphql::ShopifyQueryBuilder::pageVariables(first, after);
    (void)search_or_id;
    return executeDocument(R"graphql(query Elit21Markets($first:Int!,$after:String){markets(first:$first,after:$after){nodes{id name status primary enabled}pageInfo{hasNextPage endCursor}}})graphql", variables, "markets");
}

platform::OperationResult ShopifyMarketService::execute(const platform::OperationContext& context) {
    auto result = executeBusiness(context);
    result.metrics["client_bound"] = bound() ? 1.0 : 0.0;
    result.attributes["service"] = "ShopifyMarketService";
    result.attributes["transport"] = "Shopify Admin GraphQL";
    return result;
}

} // namespace elit21::shopify::services
