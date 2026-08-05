#include "elit21/shopify/graphql/GraphqlOperation.h"

#include <algorithm>

namespace elit21::shopify::graphql {

GraphqlOperation::GraphqlOperation()
    : platform::BusinessComponent(
          "GraphqlOperation",
          "Validated Shopify Admin GraphQL operation",
          platform::BusinessComponentSpec{"shopify", "graphql_operation", {"query"}, false, false,
                                          4U * 1024U * 1024U}) {}

GraphqlOperation::GraphqlOperation(GraphqlDocument document, Json variables, int max_attempts)
    : GraphqlOperation() {
    document_ = std::move(document);
    variables_ = std::move(variables);
    max_attempts_ = std::clamp(max_attempts, 1, 8);
}

Result<void> GraphqlOperation::validate() const {
    GraphqlRequest value(document_, variables_);
    return value.validate();
}

std::string GraphqlOperation::idempotencyKey() const {
    return GraphqlRequest(document_, variables_).idempotencyKey();
}

platform::OperationResult GraphqlOperation::execute(const platform::OperationContext& context) {
    auto validated = validate();
    if (!validated) return platform::OperationResult::failure("INVALID_GRAPHQL_OPERATION", validated.error());
    auto result = platform::OperationResult::ok("GraphQL operation validated");
    result.attributes["operation_name"] = document_.operationName();
    result.attributes["operation_type"] = document_.isMutation() ? "mutation" : "query";
    result.attributes["idempotency_key"] = idempotencyKey();
    result.attributes["mode"] = context.dry_run ? "dry_run" : "live";
    result.metrics["variable_count"] = static_cast<double>(variables_.size());
    result.metrics["max_attempts"] = static_cast<double>(max_attempts_);
    result.metrics["document_bytes"] = static_cast<double>(document_.source().size());
    return result;
}

} // namespace elit21::shopify::graphql
