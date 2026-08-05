#pragma once

#include "elit21/core/Result.h"
#include "elit21/json/Json.h"
#include "elit21/shopify/graphql/GraphqlError.h"

#include <string>
#include <vector>

namespace elit21::shopify::graphql {

class GraphqlResponse {
public:
    static Result<GraphqlResponse> parse(const std::string& body);

    [[nodiscard]] bool hasErrors() const noexcept { return !errors_.empty(); }
    [[nodiscard]] bool throttled() const;
    [[nodiscard]] bool retryable() const;
    [[nodiscard]] bool hasData() const noexcept { return !data_.isNull(); }
    [[nodiscard]] const std::vector<GraphqlError>& errors() const noexcept { return errors_; }
    [[nodiscard]] const Json& data() const noexcept { return data_; }
    [[nodiscard]] const Json& extensions() const noexcept { return extensions_; }
    [[nodiscard]] Json dataAt(const std::vector<std::string>& path) const;
    [[nodiscard]] std::string combinedErrorMessage() const;
    [[nodiscard]] double requestedCost() const;
    [[nodiscard]] double actualCost() const;
    [[nodiscard]] double currentlyAvailableCost() const;

private:
    Json data_{Json::object()};
    Json extensions_{Json::object()};
    std::vector<GraphqlError> errors_;
};

} // namespace elit21::shopify::graphql
