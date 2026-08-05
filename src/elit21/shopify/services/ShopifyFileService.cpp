#include "elit21/shopify/services/ShopifyFileService.h"

#include "elit21/shopify/graphql/ShopifyQueryBuilder.h"

#include <algorithm>

namespace elit21::shopify::services {

ShopifyFileService::ShopifyFileService()
    : platform::BusinessComponent("ShopifyFileService", "Typed Shopify Admin GraphQL service",
          platform::BusinessComponentSpec{"shopify", "query_admin_resource", {}, false, true,
                                          4U * 1024U * 1024U}) {}

ShopifyFileService::ShopifyFileService(ShopifyClient& client) : ShopifyFileService() { client_ = &client; }

ShopifyClient& ShopifyFileService::requireClient() {
    if (!client_) throw std::logic_error("ShopifyFileService is not bound to a ShopifyClient");
    return *client_;
}

Result<Json> ShopifyFileService::executeDocument(const std::string& query, const Json& variables,
                                      const std::string& response_field) {
    if (query.empty()) return Result<Json>::failure("Shopify GraphQL document is empty");
    auto result = requireClient().graphql(query, variables);
    if (!result) return result;
    if (response_field.empty()) return result;
    const auto selected = result.value().get(response_field);
    if (selected.isNull()) return Result<Json>::failure("Shopify response field is absent: " + response_field);
    return Result<Json>::success(selected);
}

Result<Json> ShopifyFileService::filesPage(int first, const std::string& after,
                              const std::string& search_or_id) {
    Json variables = shopify::graphql::ShopifyQueryBuilder::pageVariables(first, after);
    if (!search_or_id.empty()) variables.set("query", search_or_id);
    return executeDocument(R"graphql(query Elit21Files($first:Int!,$after:String,$query:String){files(first:$first,after:$after,query:$query,sortKey:UPDATED_AT){nodes{... on GenericFile{id alt fileStatus url createdAt} ... on MediaImage{id alt image{url width height} fileStatus createdAt}}pageInfo{hasNextPage endCursor}}})graphql", variables, "files");
}

platform::OperationResult ShopifyFileService::execute(const platform::OperationContext& context) {
    auto result = executeBusiness(context);
    result.metrics["client_bound"] = bound() ? 1.0 : 0.0;
    result.attributes["service"] = "ShopifyFileService";
    result.attributes["transport"] = "Shopify Admin GraphQL";
    return result;
}

} // namespace elit21::shopify::services
