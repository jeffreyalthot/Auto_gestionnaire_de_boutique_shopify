#include "elit21/shopify/graphql/GraphqlTransport.h"

#include "elit21/platform/ManagedComponent.h"
#include "elit21/shopify/ShopifyApiVersionGuard.h"
#include "elit21/util/StringUtil.h"

#include <algorithm>
#include <chrono>
#include <cmath>
#include <cstdlib>
#include <thread>

namespace elit21::shopify::graphql {
namespace {

bool isThrottleError(const GraphqlResponse& response) {
    for (const auto& error : response.errors()) {
        const auto code = util::lower(error.code);
        const auto message = util::lower(error.message);
        if (code == "throttled" || message.find("throttl") != std::string::npos) return true;
    }
    return false;
}

std::chrono::milliseconds retryAfter(const HttpResponse& response,
                                     int attempt,
                                     std::string_view request_body) {
    const auto iterator = response.headers.find("retry-after");
    if (iterator != response.headers.end()) {
        char* end = nullptr;
        const double seconds = std::strtod(iterator->second.c_str(), &end);
        if (end != iterator->second.c_str() && std::isfinite(seconds) && seconds >= 0.0) {
            return std::chrono::milliseconds(
                static_cast<long long>(std::min(30.0, seconds) * 1000.0));
        }
    }
    const auto exponent = std::min(attempt, 7);
    const auto base = 250LL * (1LL << exponent);
    const auto jitter = static_cast<long long>(platform::stableHash(request_body) % 173ULL);
    return std::chrono::milliseconds(std::min(30000LL, base + jitter));
}


std::string apiVersionFromEndpoint(const std::string& endpoint) {
    constexpr std::string_view marker{"/admin/api/"};
    const auto position = endpoint.find(marker);
    if (position == std::string::npos) return {};
    const auto begin = position + marker.size();
    const auto end = endpoint.find('/', begin);
    return endpoint.substr(begin, end == std::string::npos ? std::string::npos : end - begin);
}

std::string inferOperationName(const std::string& query) {
    const auto trimmed = util::trim(query);
    const auto first_space = trimmed.find_first_of(" \t\r\n");
    if (first_space == std::string::npos) return {};
    const auto start = trimmed.find_first_not_of(" \t\r\n", first_space);
    if (start == std::string::npos || trimmed[start] == '{') return {};
    const auto end = trimmed.find_first_of("( {\t\r\n", start);
    if (end == std::string::npos) return trimmed.substr(start);
    return trimmed.substr(start, end - start);
}

} // namespace

GraphqlTransport::GraphqlTransport(HttpClient& http, std::string endpoint, std::string access_token)
    : http_(http),
      endpoint_(std::move(endpoint)),
      access_token_(std::move(access_token)),
      requested_api_version_(apiVersionFromEndpoint(endpoint_)) {
    metrics_.requested_api_version = requested_api_version_;
}

Result<GraphqlResponse> GraphqlTransport::execute(const GraphqlRequest& request, int max_attempts) {
    return executeBody(request.dump(), max_attempts);
}

Result<GraphqlResponse> GraphqlTransport::executeRaw(const std::string& query,
                                                      const Json& variables,
                                                      int max_attempts,
                                                      const std::string& operation_name) {
    if (query.empty()) return Result<GraphqlResponse>::failure("Shopify GraphQL document is empty");
    Json body = Json::object();
    body.set("query", query);
    body.set("variables", variables);
    const auto resolved_name = operation_name.empty() ? inferOperationName(query) : operation_name;
    if (!resolved_name.empty()) body.set("operationName", resolved_name);
    return executeBody(body.dump(), max_attempts);
}

Result<GraphqlResponse> GraphqlTransport::executeBody(const std::string& body, int max_attempts) {
    if (endpoint_.rfind("https://", 0) != 0) {
        return Result<GraphqlResponse>::failure("Shopify endpoint must use HTTPS");
    }
    if (access_token_.empty()) {
        return Result<GraphqlResponse>::failure("Shopify access token is empty");
    }
    max_attempts = std::clamp(max_attempts, 1, 8);

    const std::map<std::string, std::string> headers{
        {"X-Shopify-Access-Token", access_token_},
        {"Accept", "application/json"},
        {"X-ELIT21-Request-Hash", std::to_string(platform::stableHash(body))}
    };

    std::string last_error = "Shopify GraphQL request failed";
    for (int attempt = 0; attempt < max_attempts; ++attempt) {
        if (!circuit_breaker_.allowRequest()) {
            return Result<GraphqlResponse>::failure(
                "Shopify GraphQL circuit breaker is open");
        }
        const auto throttle_delay = throttler_.recommendedDelay(25.0);
        if (throttle_delay.count() > 0) std::this_thread::sleep_for(throttle_delay);

        {
            std::lock_guard lock(metrics_mutex_);
            ++metrics_.requests;
        }
        auto response = http_.postJson(endpoint_, body, headers);
        if (!response) {
            last_error = response.error();
            circuit_breaker_.recordFailure();
            recordRetry(false, true);
            if (attempt + 1 < max_attempts) {
                HttpResponse empty;
                std::this_thread::sleep_for(retryAfter(empty, attempt, body));
                continue;
            }
            break;
        }

        recordResponse(response.value());
        auto version_check = verifyServedApiVersion(response.value());
        if (!version_check) {
            recordPermanentFailure();
            return Result<GraphqlResponse>::failure(version_check.error());
        }
        const long status = response.value().status;
        if (status == 429 || status >= 500) {
            const bool throttled = status == 429;
            if (throttled) circuit_breaker_.recordSuccess();
            else circuit_breaker_.recordFailure();
            last_error = "Shopify transient HTTP " + std::to_string(status);
            recordRetry(throttled, true);
            if (attempt + 1 < max_attempts) {
                std::this_thread::sleep_for(retryAfter(response.value(), attempt, body));
                continue;
            }
            break;
        }
        if (status < 200 || status >= 300) {
            circuit_breaker_.recordSuccess();
            recordPermanentFailure();
            return Result<GraphqlResponse>::failure(
                "Shopify GraphQL HTTP " + std::to_string(status) + ": " + response.value().body);
        }

        auto parsed = GraphqlResponse::parse(response.value().body);
        if (!parsed) {
            circuit_breaker_.recordFailure();
            recordPermanentFailure();
            return parsed;
        }
        throttler_.update(parsed.value().extensions());
        {
            std::lock_guard lock(metrics_mutex_);
            metrics_.currently_available_cost = throttler_.available();
        }
        if (!parsed.value().hasErrors()) {
            circuit_breaker_.recordSuccess();
            return parsed;
        }

        if (isThrottleError(parsed.value())) {
            circuit_breaker_.recordSuccess();
            last_error = parsed.value().combinedErrorMessage();
            recordRetry(true, false);
            if (attempt + 1 < max_attempts) {
                auto delay = throttler_.recommendedDelay(100.0);
                delay = std::max(delay, retryAfter(response.value(), attempt, body));
                std::this_thread::sleep_for(delay);
                continue;
            }
            break;
        }

        circuit_breaker_.recordSuccess();
        recordPermanentFailure();
        return Result<GraphqlResponse>::failure(parsed.value().combinedErrorMessage());
    }
    recordPermanentFailure();
    return Result<GraphqlResponse>::failure(last_error);
}


Result<void> GraphqlTransport::verifyServedApiVersion(const HttpResponse& response) {
    const auto iterator = response.headers.find("x-shopify-api-version");
    if (iterator == response.headers.end() || iterator->second.empty() ||
        requested_api_version_.empty()) {
        return Result<void>::success();
    }
    {
        std::lock_guard lock(metrics_mutex_);
        metrics_.last_served_api_version = iterator->second;
        if (iterator->second != requested_api_version_) {
            ++metrics_.api_version_mismatches;
        }
    }
    return ShopifyApiVersionGuard::requireExactServedVersion(
        requested_api_version_, iterator->second);
}

void GraphqlTransport::recordResponse(const HttpResponse& response) {
    std::lock_guard lock(metrics_mutex_);
    metrics_.last_http_status = response.status;
    metrics_.last_elapsed_seconds = response.elapsed_seconds;
    if (const auto request_id = response.headers.find("x-request-id"); request_id != response.headers.end()) {
        metrics_.last_request_id = request_id->second;
    } else if (const auto shopify_request_id = response.headers.find("x-shopify-request-id");
               shopify_request_id != response.headers.end()) {
        metrics_.last_request_id = shopify_request_id->second;
    }
    if (const auto deprecated = response.headers.find("x-shopify-api-deprecated-reason");
        deprecated != response.headers.end() && !deprecated->second.empty()) {
        ++metrics_.deprecation_warnings;
        metrics_.last_deprecation_reason = deprecated->second;
    }
}

void GraphqlTransport::recordRetry(bool throttled, bool transient_failure) {
    std::lock_guard lock(metrics_mutex_);
    ++metrics_.retries;
    if (throttled) ++metrics_.throttles;
    if (transient_failure) ++metrics_.transient_failures;
}

void GraphqlTransport::recordPermanentFailure() {
    std::lock_guard lock(metrics_mutex_);
    ++metrics_.permanent_failures;
}

GraphqlTransportMetrics GraphqlTransport::metrics() const {
    std::lock_guard lock(metrics_mutex_);
    auto output = metrics_;
    const auto circuit = circuit_breaker_.snapshot();
    output.circuit_rejections = circuit.rejected_requests;
    output.circuit_openings = circuit.openings;
    output.circuit_state = circuit.state_name;
    return output;
}

} // namespace elit21::shopify::graphql
