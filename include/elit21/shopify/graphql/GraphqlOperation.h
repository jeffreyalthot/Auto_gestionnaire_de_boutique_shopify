#pragma once

#include "elit21/core/Result.h"
#include "elit21/json/Json.h"
#include "elit21/platform/BusinessComponent.h"
#include "elit21/shopify/graphql/GraphqlRequest.h"

#include <string>

namespace elit21::shopify::graphql {

class GraphqlOperation final : public platform::BusinessComponent {
public:
    GraphqlOperation();
    GraphqlOperation(GraphqlDocument document, Json variables = Json::object(), int max_attempts = 4);

    [[nodiscard]] const GraphqlDocument& document() const noexcept { return document_; }
    [[nodiscard]] const Json& variables() const noexcept { return variables_; }
    [[nodiscard]] GraphqlRequest request() const { return GraphqlRequest(document_, variables_); }
    [[nodiscard]] int maxAttempts() const noexcept { return max_attempts_; }
    [[nodiscard]] Result<void> validate() const;
    [[nodiscard]] std::string idempotencyKey() const;
    platform::OperationResult execute(const platform::OperationContext& context) override;

private:
    GraphqlDocument document_{"Elit21Health", "query Elit21Health { shop { id } }"};
    Json variables_{Json::object()};
    int max_attempts_{4};
};

} // namespace elit21::shopify::graphql
