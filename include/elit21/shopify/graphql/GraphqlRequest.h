#pragma once

#include "elit21/core/Result.h"
#include "elit21/json/Json.h"
#include "elit21/shopify/graphql/GraphqlDocument.h"

#include <string>

namespace elit21::shopify::graphql {

class GraphqlRequest {
public:
    GraphqlRequest(GraphqlDocument document, Json variables = Json::object());

    [[nodiscard]] const GraphqlDocument& document() const noexcept { return document_; }
    [[nodiscard]] const Json& variables() const noexcept { return variables_; }
    [[nodiscard]] Result<void> validate() const;
    [[nodiscard]] Json toJson() const;
    [[nodiscard]] std::string dump() const { return toJson().dump(); }
    [[nodiscard]] std::string idempotencyKey() const;

private:
    GraphqlDocument document_;
    Json variables_;
};

} // namespace elit21::shopify::graphql
