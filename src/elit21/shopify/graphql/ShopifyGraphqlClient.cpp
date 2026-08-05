#include "elit21/shopify/graphql/ShopifyGraphqlClient.h"

namespace elit21::shopify::graphql {

Result<Json> ShopifyGraphqlClient::execute(const GraphqlDocument& document, const Json& variables) {
    GraphqlOperation operation(document, variables);
    return execute(operation);
}

Result<Json> ShopifyGraphqlClient::execute(const GraphqlOperation& operation) {
    ++metrics_.operations;
    if (operation.document().isMutation()) ++metrics_.mutations;
    else ++metrics_.queries;
    auto valid = operation.validate();
    if (!valid) {
        ++metrics_.failures;
        return Result<Json>::failure(valid.error());
    }
    auto response = transport_.execute(operation.request(), operation.maxAttempts());
    if (!response) {
        ++metrics_.failures;
        return Result<Json>::failure(response.error());
    }
    return Result<Json>::success(response.value().data());
}

Result<Json> ShopifyGraphqlClient::executeAt(const GraphqlDocument& document, const Json& variables,
                                              const std::vector<std::string>& data_path) {
    ++metrics_.operations;
    if (document.isMutation()) ++metrics_.mutations;
    else ++metrics_.queries;
    GraphqlRequest request(document, variables);
    auto valid = request.validate();
    if (!valid) {
        ++metrics_.failures;
        return Result<Json>::failure(valid.error());
    }
    auto response = transport_.execute(request);
    if (!response) {
        ++metrics_.failures;
        return Result<Json>::failure(response.error());
    }
    const auto value = response.value().dataAt(data_path);
    if (value.isNull()) {
        ++metrics_.failures;
        return Result<Json>::failure("Requested GraphQL data path is absent");
    }
    return Result<Json>::success(value);
}

} // namespace elit21::shopify::graphql
