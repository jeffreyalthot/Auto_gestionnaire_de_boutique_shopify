#include "elit21/shopify/services/ShopifyCatalogService.h"

#include "elit21/shopify/graphql/ShopifyQueryBuilder.h"

#include <algorithm>

namespace elit21::shopify::services {

ShopifyCatalogService::ShopifyCatalogService()
    : platform::BusinessComponent("ShopifyCatalogService", "Typed Shopify Admin GraphQL service",
          platform::BusinessComponentSpec{"shopify", "query_admin_resource", {}, false, true,
                                          4U * 1024U * 1024U}) {}

ShopifyCatalogService::ShopifyCatalogService(ShopifyClient& client) : ShopifyCatalogService() { client_ = &client; }

ShopifyClient& ShopifyCatalogService::requireClient() {
    if (!client_) throw std::logic_error("ShopifyCatalogService is not bound to a ShopifyClient");
    return *client_;
}

Result<Json> ShopifyCatalogService::executeDocument(const std::string& query, const Json& variables,
                                      const std::string& response_field) {
    if (query.empty()) return Result<Json>::failure("Shopify GraphQL document is empty");
    auto result = requireClient().graphql(query, variables);
    if (!result) return result;
    if (response_field.empty()) return result;
    const auto selected = result.value().get(response_field);
    if (selected.isNull()) return Result<Json>::failure("Shopify response field is absent: " + response_field);
    return Result<Json>::success(selected);
}

Result<Json> ShopifyCatalogService::catalogPage(int first, const std::string& after,
                              const std::string& search_or_id) {
    Json variables = shopify::graphql::ShopifyQueryBuilder::pageVariables(first, after);
    if (!search_or_id.empty()) variables.set("query", search_or_id);
    return executeDocument(R"graphql(query Elit21Catalog($first:Int!,$after:String,$query:String){products(first:$first,after:$after,query:$query,sortKey:UPDATED_AT){nodes{id title handle status updatedAt variants(first:100){nodes{id sku title price inventoryQuantity inventoryItem{id}}}}pageInfo{hasNextPage endCursor}}})graphql", variables, "products");
}

platform::OperationResult ShopifyCatalogService::execute(const platform::OperationContext& context) {
    auto result = executeBusiness(context);
    result.metrics["client_bound"] = bound() ? 1.0 : 0.0;
    result.attributes["service"] = "ShopifyCatalogService";
    result.attributes["transport"] = "Shopify Admin GraphQL";
    return result;
}

} // namespace elit21::shopify::services
