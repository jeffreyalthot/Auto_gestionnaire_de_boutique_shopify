#include "elit21/shopify/graphql/GraphqlRequest.h"

#include "elit21/platform/ManagedComponent.h"

namespace elit21::shopify::graphql {

GraphqlRequest::GraphqlRequest(GraphqlDocument document, Json variables)
    : document_(std::move(document)), variables_(std::move(variables)) {}

Result<void> GraphqlRequest::validate() const {
    auto document_result = document_.validate();
    if (!document_result) return document_result;
    if (!variables_.isObject()) return Result<void>::failure("GraphQL variables must be a JSON object");
    for (const auto& variable : document_.declaredVariables()) {
        if (!variables_.contains(variable)) {
            // Optional variables are legal, therefore this is intentionally not a hard failure.
            continue;
        }
    }
    return Result<void>::success();
}

Json GraphqlRequest::toJson() const {
    Json output = Json::object();
    output.set("operationName", document_.operationName());
    output.set("query", document_.source());
    output.set("variables", variables_);
    return output;
}

std::string GraphqlRequest::idempotencyKey() const {
    return document_.operationName() + '-' +
           std::to_string(platform::stableHash(document_.fingerprint() + variables_.dump()));
}

} // namespace elit21::shopify::graphql
