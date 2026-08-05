#include "elit21/shopify/graphql/GraphqlResponse.h"

#include <sstream>

namespace elit21::shopify::graphql {

Result<GraphqlResponse> GraphqlResponse::parse(const std::string& body) {
    if (body.empty()) return Result<GraphqlResponse>::failure("GraphQL response body is empty");
    auto parsed = Json::parse(body);
    if (!parsed) return Result<GraphqlResponse>::failure(parsed.error());
    if (!parsed.value().isObject()) {
        return Result<GraphqlResponse>::failure("GraphQL response must be a JSON object");
    }

    GraphqlResponse response;
    response.data_ = parsed.value().get("data");
    response.extensions_ = parsed.value().get("extensions");
    const auto errors = parsed.value().get("errors");
    if (!errors.isNull() && !errors.isArray()) {
        return Result<GraphqlResponse>::failure("GraphQL errors member must be an array");
    }
    for (std::size_t index = 0; index < errors.size(); ++index) {
        const auto value = errors.at(index);
        GraphqlError error;
        error.message = value.getString("message", "Unknown GraphQL error");
        error.extensions = value.get("extensions");
        error.code = error.extensions.getString("code");
        const auto path = value.get("path");
        for (std::size_t part = 0; part < path.size(); ++part) {
            error.path.push_back(path.at(part).scalarStringValue());
        }
        response.errors_.push_back(std::move(error));
    }
    if (!parsed.value().contains("data") && response.errors_.empty()) {
        return Result<GraphqlResponse>::failure("GraphQL response contains neither data nor errors");
    }
    return Result<GraphqlResponse>::success(std::move(response));
}

bool GraphqlResponse::throttled() const {
    for (const auto& error : errors_) if (error.throttled()) return true;
    return false;
}

bool GraphqlResponse::retryable() const {
    if (errors_.empty()) return false;
    for (const auto& error : errors_) if (!error.retryable()) return false;
    return true;
}

Json GraphqlResponse::dataAt(const std::vector<std::string>& path) const {
    Json current = data_;
    for (const auto& part : path) {
        if (!current.isObject() || !current.contains(part)) return Json{};
        current = current.get(part);
    }
    return current;
}

std::string GraphqlResponse::combinedErrorMessage() const {
    std::ostringstream output;
    for (std::size_t index = 0; index < errors_.size(); ++index) {
        if (index) output << "; ";
        output << errors_[index].describe();
    }
    return output.str();
}

double GraphqlResponse::requestedCost() const {
    return extensions_.get("cost").getNumber("requestedQueryCost", 0.0);
}

double GraphqlResponse::actualCost() const {
    return extensions_.get("cost").getNumber("actualQueryCost", 0.0);
}

double GraphqlResponse::currentlyAvailableCost() const {
    return extensions_.get("cost").get("throttleStatus").getNumber("currentlyAvailable", 0.0);
}

} // namespace elit21::shopify::graphql
