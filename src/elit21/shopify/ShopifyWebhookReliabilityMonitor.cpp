#include "elit21/shopify/ShopifyWebhookReliabilityMonitor.h"

#include "elit21/storage/Database.h"

#include <algorithm>
#include <cmath>
#include <sstream>

namespace elit21::shopify {

Json ShopifyWebhookDeliverySample::toJson() const {
    Json output = Json::object();
    output.set("topic", topic);
    output.set("http_status", static_cast<std::int64_t>(http_status));
    output.set("response_time_ms", response_time_ms);
    output.set("retry_count", retry_count);
    output.set("subscription_removed", subscription_removed);
    return output;
}

Result<ShopifyWebhookDeliverySample> ShopifyWebhookDeliverySample::fromJson(const Json& value) {
    if (!value.isObject()) {
        return Result<ShopifyWebhookDeliverySample>::failure("Webhook delivery sample must be an object");
    }
    ShopifyWebhookDeliverySample sample;
    sample.topic = value.getString("topic");
    sample.http_status = static_cast<long>(value.getInt64("http_status"));
    sample.response_time_ms = value.getNumber("response_time_ms", -1.0);
    sample.retry_count = value.getInt("retry_count", 0);
    sample.subscription_removed = value.getBool("subscription_removed", false);
    if (sample.topic.empty() || sample.http_status < 0 || sample.http_status > 599 ||
        !std::isfinite(sample.response_time_ms) || sample.response_time_ms < 0.0 ||
        sample.retry_count < 0 || sample.retry_count > 8) {
        return Result<ShopifyWebhookDeliverySample>::failure("Invalid webhook delivery sample");
    }
    return Result<ShopifyWebhookDeliverySample>::success(std::move(sample));
}

Json ShopifyWebhookReliabilityReport::toJson() const {
    Json output = Json::object();
    output.set("deliveries", static_cast<std::int64_t>(deliveries));
    output.set("successful", static_cast<std::int64_t>(successful));
    output.set("failed", static_cast<std::int64_t>(failed));
    output.set("removed_subscriptions", static_cast<std::int64_t>(removed_subscriptions));
    output.set("maximum_retry_count", maximum_retry_count);
    output.set("failure_rate_percent", failure_rate_percent);
    output.set("p90_response_time_ms", p90_response_time_ms);
    output.set("healthy", healthy);
    Json list = Json::array();
    for (const auto& finding : findings) list.push(Json(json_object_new_string(finding.c_str()), true));
    output.set("findings", list);
    return output;
}

std::string ShopifyWebhookReliabilityReport::markdown() const {
    std::ostringstream output;
    output << "# Shopify Webhook Reliability\n\n"
           << "- Deliveries: **" << deliveries << "**\n"
           << "- Failure rate: **" << failure_rate_percent << "%**\n"
           << "- p90 response time: **" << p90_response_time_ms << " ms**\n"
           << "- Maximum retry count: **" << maximum_retry_count << "**\n"
           << "- Removed subscriptions: **" << removed_subscriptions << "**\n"
           << "- Healthy: **" << (healthy ? "yes" : "no") << "**\n\n";
    for (const auto& finding : findings) output << "- " << finding << "\n";
    return output.str();
}

ShopifyWebhookReliabilityReport ShopifyWebhookReliabilityMonitor::analyze(
    const std::vector<ShopifyWebhookDeliverySample>& samples,
    double maximum_failure_rate_percent,
    double maximum_p90_response_time_ms) {
    ShopifyWebhookReliabilityReport report;
    report.deliveries = samples.size();
    if (samples.empty()) {
        report.healthy = false;
        report.findings.push_back("No webhook delivery sample is available");
        return report;
    }

    std::vector<double> timings;
    timings.reserve(samples.size());
    for (const auto& sample : samples) {
        timings.push_back(sample.response_time_ms);
        if (sample.successful()) ++report.successful;
        else ++report.failed;
        if (sample.subscription_removed) ++report.removed_subscriptions;
        report.maximum_retry_count = std::max(report.maximum_retry_count, sample.retry_count);
    }
    std::sort(timings.begin(), timings.end());
    const auto rank = static_cast<std::size_t>(
        std::ceil(0.90 * static_cast<double>(timings.size()))) - 1U;
    report.p90_response_time_ms = timings[std::min(rank, timings.size() - 1U)];
    report.failure_rate_percent =
        100.0 * static_cast<double>(report.failed) / static_cast<double>(report.deliveries);

    if (report.failure_rate_percent > maximum_failure_rate_percent) {
        report.findings.push_back("Webhook failure rate exceeds the configured 0.5% operational target");
    }
    if (report.p90_response_time_ms >= maximum_p90_response_time_ms) {
        report.findings.push_back("Webhook p90 response time is too close to Shopify's five-second timeout");
    }
    if (report.maximum_retry_count >= 8) {
        report.findings.push_back("At least one delivery exhausted Shopify's retry budget");
    }
    if (report.removed_subscriptions > 0) {
        report.findings.push_back("A webhook subscription was removed and must be reconciled");
    }
    report.healthy = report.findings.empty();
    return report;
}

Result<std::vector<ShopifyWebhookDeliverySample>> ShopifyWebhookReliabilityMonitor::parseSamples(
    const Json& input) {
    const auto array = input.isArray() ? input : input.get("deliveries");
    if (!array.isArray()) {
        return Result<std::vector<ShopifyWebhookDeliverySample>>::failure(
            "Webhook reliability input must be an array or contain a deliveries array");
    }
    std::vector<ShopifyWebhookDeliverySample> samples;
    samples.reserve(array.size());
    for (std::size_t index = 0; index < array.size(); ++index) {
        auto sample = ShopifyWebhookDeliverySample::fromJson(array.at(index));
        if (!sample) {
            return Result<std::vector<ShopifyWebhookDeliverySample>>::failure(
                "Invalid delivery at index " + std::to_string(index) + ": " + sample.error());
        }
        samples.push_back(sample.take());
    }
    return Result<std::vector<ShopifyWebhookDeliverySample>>::success(std::move(samples));
}

Result<ShopifyWebhookReliabilityReport> ShopifyWebhookReliabilityMonitor::analyzeAndPersist(
    const std::vector<ShopifyWebhookDeliverySample>& samples,
    Database& database) {
    auto report = analyze(samples);
    auto stored = database.recordShopifyWebhookSloAudit(
        report.deliveries, report.failure_rate_percent, report.p90_response_time_ms,
        report.removed_subscriptions, report.healthy, report.toJson().dump());
    if (!stored) return Result<ShopifyWebhookReliabilityReport>::failure(stored.error());
    return Result<ShopifyWebhookReliabilityReport>::success(std::move(report));
}

} // namespace elit21::shopify
