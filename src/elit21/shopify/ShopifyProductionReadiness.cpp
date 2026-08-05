#include "elit21/shopify/ShopifyProductionReadiness.h"

#include "elit21/shopify/ShopifyApiVersion.h"
#include "elit21/shopify/ShopifyEndpoints.h"

#include <algorithm>
#include <array>
#include <set>

namespace elit21::shopify {
namespace {
void add(ShopifyReadinessReport& report, std::string id, std::string level,
         std::string message, bool passed, int weight) {
    report.checks.push_back({std::move(id), std::move(level), std::move(message), passed, weight});
}
} // namespace

Json ShopifyReadinessCheck::toJson() const {
    Json output = Json::object();
    output.set("id", id); output.set("level", level); output.set("message", message);
    output.set("passed", passed); output.set("weight", weight); return output;
}

Json ShopifyReadinessReport::toJson() const {
    Json output = Json::object(); output.set("score", score); output.set("production_ready", production_ready);
    Json values = Json::array(); for (const auto& check : checks) values.push(check.toJson());
    output.set("checks", values); return output;
}

ShopifyReadinessReport ShopifyProductionReadiness::evaluate(const Config& config, const Database* database) {
    ShopifyReadinessReport report;
    const auto valid_shop = ShopifyEndpoints::validateShop(config.shopify.shop);
    add(report, "shop_domain", "critical", "Canonical myshopify.com shop identifier", bool(valid_shop), 10);
    const auto version = ShopifyApiVersion::parse(config.shopify.api_version);
    add(report, "api_version", "critical", "Quarterly Shopify Admin API version is syntactically valid", bool(version), 8);
    add(report, "admin_token", "critical", "Admin API access token is configured", config.shopify.access_token.size() >= 16, 12);
    add(report, "webhook_secret", "critical", "Webhook signing secret has sufficient entropy", config.shopify.webhook_secret.size() >= 24, 12);
    add(report, "webhook_https", "critical", "Webhook public base URL uses HTTPS",
        ShopifyEndpoints::isHttpsUrl(config.shopify.webhook_base_url), 10);
    add(report, "tls", "critical", "TLS certificate verification is enabled", config.network.verify_tls, 10);
    add(report, "currency", "critical", "Store integration is locked to CAD", config.aliexpress.currency == "CAD", 6);
    add(report, "country", "critical", "Supplier destination is locked to Canada", config.aliexpress.country == "CA", 6);
    add(report, "markup", "critical", "Markup is at least 100% before shipping",
        config.pricing.markup_percent_before_shipping >= 100.0, 10);
    add(report, "database", "critical", "Transactional SQLite database is open", database && database->isOpen(), 8);
    add(report, "live_mode", "warning", "Live mode and dry-run flags are mutually consistent",
        config.app.live_orders != config.app.dry_run, 4);
    add(report, "aliexpress_credentials", "critical", "AliExpress application and access token are configured",
        !config.aliexpress.app_key.empty() && !config.aliexpress.app_secret.empty() &&
        !config.aliexpress.access_token.empty(), 4);

    int passed_weight = 0, total_weight = 0; bool critical_failure = false;
    for (const auto& check : report.checks) {
        total_weight += check.weight;
        if (check.passed) passed_weight += check.weight;
        else if (check.level == "critical") critical_failure = true;
    }
    report.score = total_weight == 0 ? 0 : (passed_weight * 100 + total_weight / 2) / total_weight;
    report.production_ready = !critical_failure && report.score >= 90;
    return report;
}

} // namespace elit21::shopify
