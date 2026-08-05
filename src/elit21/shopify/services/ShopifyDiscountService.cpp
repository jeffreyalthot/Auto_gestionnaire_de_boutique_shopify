#include "elit21/shopify/services/ShopifyDiscountService.h"

#include "elit21/shopify/graphql/ShopifyQueryBuilder.h"

#include <algorithm>

namespace elit21::shopify::services {

ShopifyDiscountService::ShopifyDiscountService()
    : platform::BusinessComponent("ShopifyDiscountService", "Typed Shopify Admin GraphQL service",
          platform::BusinessComponentSpec{"shopify", "query_admin_resource", {}, false, true,
                                          4U * 1024U * 1024U}) {}

ShopifyDiscountService::ShopifyDiscountService(ShopifyClient& client) : ShopifyDiscountService() { client_ = &client; }

ShopifyClient& ShopifyDiscountService::requireClient() {
    if (!client_) throw std::logic_error("ShopifyDiscountService is not bound to a ShopifyClient");
    return *client_;
}

Result<Json> ShopifyDiscountService::executeDocument(const std::string& query, const Json& variables,
                                      const std::string& response_field) {
    if (query.empty()) return Result<Json>::failure("Shopify GraphQL document is empty");
    auto result = requireClient().graphql(query, variables);
    if (!result) return result;
    if (response_field.empty()) return result;
    const auto selected = result.value().get(response_field);
    if (selected.isNull()) return Result<Json>::failure("Shopify response field is absent: " + response_field);
    return Result<Json>::success(selected);
}

Result<Json> ShopifyDiscountService::discountsPage(int first, const std::string& after,
                              const std::string& search_or_id) {
    Json variables = shopify::graphql::ShopifyQueryBuilder::pageVariables(first, after);
    (void)search_or_id;
    return executeDocument(R"graphql(query Elit21Discounts($first:Int!,$after:String){discountNodes(first:$first,after:$after){nodes{id discount{... on DiscountCodeBasic{title status startsAt endsAt}}}pageInfo{hasNextPage endCursor}}})graphql", variables, "discountNodes");
}

platform::OperationResult ShopifyDiscountService::execute(const platform::OperationContext& context) {
    auto result = executeBusiness(context);
    result.metrics["client_bound"] = bound() ? 1.0 : 0.0;
    result.attributes["service"] = "ShopifyDiscountService";
    result.attributes["transport"] = "Shopify Admin GraphQL";
    return result;
}

} // namespace elit21::shopify::services
