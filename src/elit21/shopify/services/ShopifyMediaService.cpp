#include "elit21/shopify/services/ShopifyMediaService.h"

#include "elit21/shopify/graphql/ShopifyQueryBuilder.h"

#include <algorithm>

namespace elit21::shopify::services {

ShopifyMediaService::ShopifyMediaService()
    : platform::BusinessComponent("ShopifyMediaService", "Typed Shopify Admin GraphQL service",
          platform::BusinessComponentSpec{"shopify", "query_admin_resource", {}, false, true,
                                          4U * 1024U * 1024U}) {}

ShopifyMediaService::ShopifyMediaService(ShopifyClient& client) : ShopifyMediaService() { client_ = &client; }

ShopifyClient& ShopifyMediaService::requireClient() {
    if (!client_) throw std::logic_error("ShopifyMediaService is not bound to a ShopifyClient");
    return *client_;
}

Result<Json> ShopifyMediaService::executeDocument(const std::string& query, const Json& variables,
                                      const std::string& response_field) {
    if (query.empty()) return Result<Json>::failure("Shopify GraphQL document is empty");
    auto result = requireClient().graphql(query, variables);
    if (!result) return result;
    if (response_field.empty()) return result;
    const auto selected = result.value().get(response_field);
    if (selected.isNull()) return Result<Json>::failure("Shopify response field is absent: " + response_field);
    return Result<Json>::success(selected);
}

Result<Json> ShopifyMediaService::productMedia(int first, const std::string& after,
                              const std::string& search_or_id) {
    Json variables = shopify::graphql::ShopifyQueryBuilder::pageVariables(first, after);
    if (search_or_id.empty()) return Result<Json>::failure("ShopifyMediaService requires a Shopify GID");
    variables.set("id", search_or_id);
    return executeDocument(R"graphql(query Elit21ProductMedia($id:ID!,$first:Int!,$after:String){product(id:$id){id media(first:$first,after:$after){nodes{... on MediaImage{id alt status image{url width height}} ... on Video{id alt status sources{url mimeType}}}pageInfo{hasNextPage endCursor}}}})graphql", variables, "product");
}

platform::OperationResult ShopifyMediaService::execute(const platform::OperationContext& context) {
    auto result = executeBusiness(context);
    result.metrics["client_bound"] = bound() ? 1.0 : 0.0;
    result.attributes["service"] = "ShopifyMediaService";
    result.attributes["transport"] = "Shopify Admin GraphQL";
    return result;
}

} // namespace elit21::shopify::services
