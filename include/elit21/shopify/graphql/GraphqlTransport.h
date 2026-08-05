#pragma once

#include "elit21/core/Result.h"
#include "elit21/json/Json.h"
#include "elit21/net/HttpClient.h"
#include "elit21/shopify/graphql/GraphqlRequest.h"
#include "elit21/shopify/graphql/GraphqlResponse.h"
#include "elit21/shopify/graphql/ShopifyCostThrottler.h"
#include "elit21/shopify/graphql/ShopifyGraphqlCircuitBreaker.h"

#include <cstdint>
#include <mutex>
#include <string>

namespace elit21::shopify::graphql {

struct GraphqlTransportMetrics {
    std::uint64_t requests{0};
    std::uint64_t retries{0};
    std::uint64_t throttles{0};
    std::uint64_t transient_failures{0};
    std::uint64_t permanent_failures{0};
    std::uint64_t api_version_mismatches{0};
    std::uint64_t deprecation_warnings{0};
    std::uint64_t circuit_rejections{0};
    std::uint64_t circuit_openings{0};
    long last_http_status{0};
    double last_elapsed_seconds{0.0};
    double currently_available_cost{1000.0};
    std::string requested_api_version;
    std::string last_served_api_version;
    std::string last_request_id;
    std::string last_deprecation_reason;
    std::string circuit_state{"closed"};
};

class GraphqlTransport {
public:
    GraphqlTransport(HttpClient& http, std::string endpoint, std::string access_token);

    Result<GraphqlResponse> execute(const GraphqlRequest& request, int max_attempts = 4);
    Result<GraphqlResponse> executeRaw(const std::string& query,
                                       const Json& variables = Json::object(),
                                       int max_attempts = 4,
                                       const std::string& operation_name = {});

    ShopifyCostThrottler& throttler() { return throttler_; }
    [[nodiscard]] GraphqlTransportMetrics metrics() const;

private:
    Result<GraphqlResponse> executeBody(const std::string& body, int max_attempts);
    Result<void> verifyServedApiVersion(const HttpResponse& response);
    void recordResponse(const HttpResponse& response);
    void recordRetry(bool throttled, bool transient_failure);
    void recordPermanentFailure();

    HttpClient& http_;
    std::string endpoint_;
    std::string access_token_;
    std::string requested_api_version_;
    ShopifyCostThrottler throttler_;
    ShopifyGraphqlCircuitBreaker circuit_breaker_;
    mutable std::mutex metrics_mutex_;
    GraphqlTransportMetrics metrics_;
};

} // namespace elit21::shopify::graphql
