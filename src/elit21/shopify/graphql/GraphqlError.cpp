#include "elit21/shopify/graphql/GraphqlError.h"

#include "elit21/util/StringUtil.h"

#include <algorithm>
#include <sstream>

namespace elit21::shopify::graphql {

Result<GraphqlError> GraphqlError::fromJson(const Json& value) {
    if (!value.isObject()) return Result<GraphqlError>::failure("GraphQL error must be an object");
    GraphqlError error;
    error.message = value.getString("message");
    if (error.message.empty()) return Result<GraphqlError>::failure("GraphQL error message is missing");
    const auto path_value = value.get("path");
    if (path_value.isArray()) {
        for (std::size_t index = 0; index < path_value.size(); ++index)
            error.path.push_back(path_value.at(index).scalarStringValue());
    }
    error.extensions = value.get("extensions");
    if (!error.extensions.isObject()) error.extensions = Json::object();
    error.code = error.extensions.getString("code", value.getString("code"));
    return Result<GraphqlError>::success(std::move(error));
}

bool GraphqlError::throttled() const {
    const auto normalized_code = util::lower(code);
    const auto normalized_message = util::lower(message);
    return normalized_code == "throttled" || normalized_code == "throttle_exceeded" ||
           normalized_message.find("throttl") != std::string::npos;
}

bool GraphqlError::retryable() const {
    const auto normalized_code = util::lower(code);
    return throttled() || normalized_code == "internal_server_error" ||
           normalized_code == "service_unavailable" || normalized_code == "timeout" ||
           normalized_code == "temporarily_unavailable";
}

std::chrono::milliseconds GraphqlError::retryDelay(std::chrono::milliseconds fallback) const {
    const double seconds = extensions.getNumber("retryAfter",
        extensions.getNumber("retry_after", fallback.count() / 1000.0));
    const auto bounded = std::clamp(seconds, 0.05, 300.0);
    return std::chrono::milliseconds(static_cast<long long>(bounded * 1000.0));
}

std::string GraphqlError::pathString() const {
    std::ostringstream output;
    for (std::size_t index = 0; index < path.size(); ++index) {
        if (index) output << '.';
        output << path[index];
    }
    return output.str();
}

std::string GraphqlError::describe() const {
    std::ostringstream output;
    if (!path.empty()) output << pathString() << ": ";
    output << message;
    if (!code.empty()) output << " [" << code << ']';
    return output.str();
}

} // namespace elit21::shopify::graphql
