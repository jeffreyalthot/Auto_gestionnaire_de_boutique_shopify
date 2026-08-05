#include "elit21/shopify/services/ShopifyMetafieldService.h"

#include "elit21/shopify/graphql/ShopifyQueryBuilder.h"

#include <algorithm>

namespace elit21::shopify::services {

ShopifyMetafieldService::ShopifyMetafieldService()
    : platform::BusinessComponent("ShopifyMetafieldService", "Typed Shopify Admin GraphQL service",
          platform::BusinessComponentSpec{"shopify", "query_admin_resource", {}, false, true,
                                          4U * 1024U * 1024U}) {}

ShopifyMetafieldService::ShopifyMetafieldService(ShopifyClient& client) : ShopifyMetafieldService() { client_ = &client; }

ShopifyClient& ShopifyMetafieldService::requireClient() {
    if (!client_) throw std::logic_error("ShopifyMetafieldService is not bound to a ShopifyClient");
    return *client_;
}

Result<Json> ShopifyMetafieldService::executeDocument(const std::string& query, const Json& variables,
                                      const std::string& response_field) {
    if (query.empty()) return Result<Json>::failure("Shopify GraphQL document is empty");
    auto result = requireClient().graphql(query, variables);
    if (!result) return result;
    if (response_field.empty()) return result;
    const auto selected = result.value().get(response_field);
    if (selected.isNull()) return Result<Json>::failure("Shopify response field is absent: " + response_field);
    return Result<Json>::success(selected);
}

Result<Json> ShopifyMetafieldService::ownerMetafields(int first, const std::string& after,
                              const std::string& search_or_id) {
    Json variables = shopify::graphql::ShopifyQueryBuilder::pageVariables(first, after);
    if (search_or_id.empty()) return Result<Json>::failure("ShopifyMetafieldService requires a Shopify GID");
    variables.set("owner", search_or_id);
    variables.set("namespace", "elit21");
    return executeDocument(R"graphql(query Elit21Metafields($owner:ID!,$first:Int!,$after:String,$namespace:String){node(id:$owner){... on HasMetafields{metafields(first:$first,after:$after,namespace:$namespace){nodes{id namespace key type value updatedAt}pageInfo{hasNextPage endCursor}}}}})graphql", variables, "node");
}

platform::OperationResult ShopifyMetafieldService::execute(const platform::OperationContext& context) {
    auto result = executeBusiness(context);
    result.metrics["client_bound"] = bound() ? 1.0 : 0.0;
    result.attributes["service"] = "ShopifyMetafieldService";
    result.attributes["transport"] = "Shopify Admin GraphQL";
    return result;
}

} // namespace elit21::shopify::services
