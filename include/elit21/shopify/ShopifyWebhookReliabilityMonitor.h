#pragma once

#include "elit21/core/Result.h"
#include "elit21/json/Json.h"

#include <cstddef>
#include <string>
#include <vector>

namespace elit21 {
class Database;
}

namespace elit21::shopify {

struct ShopifyWebhookDeliverySample {
    std::string topic;
    long http_status{0};
    double response_time_ms{0.0};
    int retry_count{0};
    bool subscription_removed{false};

    [[nodiscard]] bool successful() const noexcept {
        return http_status >= 200 && http_status < 300;
    }
    [[nodiscard]] Json toJson() const;
    [[nodiscard]] static Result<ShopifyWebhookDeliverySample> fromJson(const Json& value);
};

struct ShopifyWebhookReliabilityReport {
    std::size_t deliveries{0};
    std::size_t successful{0};
    std::size_t failed{0};
    std::size_t removed_subscriptions{0};
    int maximum_retry_count{0};
    double failure_rate_percent{0.0};
    double p90_response_time_ms{0.0};
    bool healthy{true};
    std::vector<std::string> findings;

    [[nodiscard]] Json toJson() const;
    [[nodiscard]] std::string markdown() const;
};

class ShopifyWebhookReliabilityMonitor {
public:
    [[nodiscard]] static ShopifyWebhookReliabilityReport analyze(
        const std::vector<ShopifyWebhookDeliverySample>& samples,
        double maximum_failure_rate_percent = 0.5,
        double maximum_p90_response_time_ms = 4000.0);

    [[nodiscard]] static Result<std::vector<ShopifyWebhookDeliverySample>> parseSamples(
        const Json& input);

    [[nodiscard]] static Result<ShopifyWebhookReliabilityReport> analyzeAndPersist(
        const std::vector<ShopifyWebhookDeliverySample>& samples,
        Database& database);
};

} // namespace elit21::shopify
