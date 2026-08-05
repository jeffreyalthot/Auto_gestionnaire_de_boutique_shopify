#pragma once

#include "elit21/core/Result.h"
#include "elit21/json/Json.h"

#include <chrono>
#include <string>
#include <vector>

namespace elit21::shopify::graphql {

struct GraphqlError {
    std::string message;
    std::vector<std::string> path;
    std::string code;
    Json extensions{Json::object()};

    static Result<GraphqlError> fromJson(const Json& value);
    [[nodiscard]] bool throttled() const;
    [[nodiscard]] bool retryable() const;
    [[nodiscard]] std::chrono::milliseconds retryDelay(std::chrono::milliseconds fallback =
        std::chrono::milliseconds(1000)) const;
    [[nodiscard]] std::string pathString() const;
    [[nodiscard]] std::string describe() const;
};

} // namespace elit21::shopify::graphql
