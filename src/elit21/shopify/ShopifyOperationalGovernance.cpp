#include "elit21/shopify/ShopifyOperationalGovernance.h"

#include "elit21/shopify/ShopifyApiVersionCatalog.h"
#include "elit21/shopify/ShopifyApiVersionGuard.h"
#include "elit21/shopify/ShopifyClient.h"
#include "elit21/shopify/ShopifyScopes.h"
#include "elit21/shopify/ShopifyWebhookRegistry.h"
#include "elit21/shopify/mutations/InventoryMutations.h"
#include "elit21/storage/Database.h"
#include "elit21/util/TimeUtil.h"

#include <algorithm>
#include <regex>

namespace elit21::shopify {
namespace {

void add(ShopifyGovernanceReport& report,
         std::string name,
         bool passed,
         ShopifyGovernanceSeverity severity,
         std::string detail) {
    report.checks.push_back({std::move(name), passed, severity, std::move(detail)});
}

void finalize(ShopifyGovernanceReport& report) {
    int earned = 0;
    int possible = 0;
    bool critical_failure = false;
    for (const auto& check : report.checks) {
        const int weight = check.severity == ShopifyGovernanceSeverity::critical ? 5 :
                           check.severity == ShopifyGovernanceSeverity::warning ? 2 : 1;
        possible += weight;
        if (check.passed) earned += weight;
        if (!check.passed && check.severity == ShopifyGovernanceSeverity::critical) critical_failure = true;
    }
    report.score = possible == 0 ? 0 : static_cast<int>((earned * 100 + possible / 2) / possible);
    report.production_ready = !critical_failure && report.score >= 90;
}

bool myShopifyDomain(const std::string& value) {
    static const std::regex subdomain_pattern("^[a-z0-9][a-z0-9-]{1,61}[a-z0-9]$");
    constexpr std::string_view suffix{".myshopify.com"};
    if (std::regex_match(value, subdomain_pattern)) return true;
    return value.size() > suffix.size() &&
           value.compare(value.size() - suffix.size(), suffix.size(), suffix) == 0 &&
           std::regex_match(value.substr(0, value.size() - suffix.size()), subdomain_pattern);
}

} // namespace

Json ShopifyGovernanceCheck::toJson() const {
    Json output = Json::object();
    output.set("name", name);
    output.set("passed", passed);
    output.set("severity", ShopifyOperationalGovernance::severityName(severity));
    output.set("detail", detail);
    return output;
}

Json ShopifyGovernanceReport::toJson() const {
    Json output = Json::object();
    output.set("score", score);
    output.set("production_ready", production_ready);
    output.set("evaluated_at", evaluated_at);
    Json values = Json::array();
    for (const auto& check : checks) values.push(check.toJson());
    output.set("checks", values);
    return output;
}

ShopifyGovernanceReport ShopifyOperationalGovernance::evaluateOffline(
    const Config& config,
    Database* database) {
    ShopifyGovernanceReport report;
    report.evaluated_at = util::utcNowIso();

    const auto config_validation = config.validate();
    add(report, "configuration", bool(config_validation), ShopifyGovernanceSeverity::critical,
        config_validation ? "configuration globale valide" : config_validation.error());
    add(report, "shop_domain", myShopifyDomain(config.shopify.shop), ShopifyGovernanceSeverity::critical,
        myShopifyDomain(config.shopify.shop) ? "domaine myshopify.com valide" : "domaine Shopify absent ou invalide");
    add(report, "graphql_only", config.shopify.graphqlEndpoint().find("/graphql.json") != std::string::npos,
        ShopifyGovernanceSeverity::critical, "Admin GraphQL utilisé comme transport principal");

    auto lifecycle = ShopifyApiVersionGuard::assess(config.shopify.api_version);
    add(report, "api_version_lifecycle", bool(lifecycle) && lifecycle.value().usable(),
        ShopifyGovernanceSeverity::critical,
        lifecycle ? lifecycle.value().toJson().dump() : lifecycle.error());
    add(report, "current_stable_version",
        config.shopify.api_version == ShopifyApiVersionGuard::currentStable(),
        ShopifyGovernanceSeverity::warning,
        "version configurée=" + config.shopify.api_version +
        ", version stable intégrée=" + ShopifyApiVersionGuard::currentStable());

    const bool credentials = !config.shopify.access_token.empty() && !config.shopify.webhook_secret.empty();
    add(report, "credentials", credentials || !config.app.live_orders,
        config.app.live_orders ? ShopifyGovernanceSeverity::critical : ShopifyGovernanceSeverity::warning,
        credentials ? "jeton et secret webhook présents" : "identifiants absents; acceptable uniquement en dry-run");
    add(report, "safe_mode", !(config.app.live_orders && config.app.dry_run),
        ShopifyGovernanceSeverity::critical,
        "dry_run et live_orders ne doivent pas être actifs simultanément");
    add(report, "webhook_https", config.shopify.webhook_base_url.rfind("https://", 0) == 0 || !config.app.live_orders,
        config.app.live_orders ? ShopifyGovernanceSeverity::critical : ShopifyGovernanceSeverity::warning,
        "URL webhook HTTPS requise en production");

    const auto inventory_document = mutations::InventoryMutations::document().source();
    add(report, "inventory_compare_and_set",
        inventory_document.find("changeFromQuantity") != std::string::npos,
        ShopifyGovernanceSeverity::critical,
        "inventorySetQuantities doit utiliser changeFromQuantity");
    add(report, "inventory_idempotency",
        inventory_document.find("@idempotent") != std::string::npos &&
        inventory_document.find("$idempotencyKey") != std::string::npos,
        ShopifyGovernanceSeverity::critical,
        "clé d'idempotence requise sur inventorySetQuantities");
    add(report, "privacy_webhooks", ShopifyWebhookRegistry::requiredTopics().size() >= 12,
        ShopifyGovernanceSeverity::critical,
        "webhooks métier et confidentialité déclarés");
    add(report, "scope_plan", ShopifyScopes::autonomousManagerDefaults().size() >= 15,
        ShopifyGovernanceSeverity::warning,
        "plan de scopes autonome complet");
    add(report, "database", database == nullptr || database->isOpen(),
        ShopifyGovernanceSeverity::critical,
        database == nullptr ? "base non demandée pour cette analyse" : "base SQLite ouverte");

    if (database != nullptr && database->isOpen()) {
        auto cached = ShopifyApiVersionCatalog::load(*database, config.shopify.shop);
        const bool catalog_ok = cached && (!cached.value() || cached.value()->supports(config.shopify.api_version));
        add(report, "cached_api_catalog", catalog_ok, ShopifyGovernanceSeverity::warning,
            !cached ? cached.error() : cached.value() ? "catalogue publicApiVersions en cache" : "catalogue non encore découvert");
    }

    finalize(report);
    if (database != nullptr && database->isOpen()) {
        database->recordShopifyGovernanceRun(
            config.shopify.shop, "offline", report.score, report.production_ready,
            config.shopify.api_version, report.toJson().dump());
    }
    return report;
}

Result<ShopifyGovernanceReport> ShopifyOperationalGovernance::evaluateLive(
    const Config& config,
    ShopifyClient& client,
    Database& database) {
    auto report = evaluateOffline(config, &database);
    auto health = client.healthCheck();
    add(report, "live_health", bool(health), ShopifyGovernanceSeverity::critical,
        health ? "requête ShopHealth réussie" : health.error());

    auto catalog = ShopifyApiVersionCatalog::discover(client);
    add(report, "public_api_versions", bool(catalog), ShopifyGovernanceSeverity::critical,
        catalog ? "catalogue publicApiVersions reçu" : catalog.error());
    if (catalog) {
        auto supported = ShopifyApiVersionCatalog::requireSupported(catalog.value(), config.shopify.api_version);
        add(report, "configured_version_supported", bool(supported), ShopifyGovernanceSeverity::critical,
            supported ? "version configurée déclarée supported par Shopify" : supported.error());
        auto persisted = ShopifyApiVersionCatalog::persist(database, catalog.value());
        add(report, "version_catalog_persisted", bool(persisted), ShopifyGovernanceSeverity::warning,
            persisted ? "catalogue versionné persisté" : persisted.error());
        const auto latest = catalog.value().latestSupported().value_or("");
        auto observed = database.recordShopifyApiVersionObservation(
            config.shopify.shop, config.shopify.api_version, latest,
            catalog.value().supports(config.shopify.api_version), catalog.value().toJson().dump());
        add(report, "version_observation_persisted", bool(observed), ShopifyGovernanceSeverity::warning,
            observed ? "observation de version persistée" : observed.error());
    }
    const auto metrics = client.apiMetrics();
    add(report, "no_version_fall_forward", metrics.api_version_mismatches == 0,
        ShopifyGovernanceSeverity::critical,
        "écarts de version détectés=" + std::to_string(metrics.api_version_mismatches));
    add(report, "no_deprecation_warning", metrics.deprecation_warnings == 0,
        ShopifyGovernanceSeverity::warning,
        metrics.last_deprecation_reason.empty() ? "aucun avertissement de dépréciation" : metrics.last_deprecation_reason);
    finalize(report);
    auto recorded = database.recordShopifyGovernanceRun(
        config.shopify.shop, "live", report.score, report.production_ready,
        config.shopify.api_version, report.toJson().dump());
    if (!recorded) {
        add(report, "governance_history", false, ShopifyGovernanceSeverity::warning, recorded.error());
        finalize(report);
    }
    return Result<ShopifyGovernanceReport>::success(std::move(report));
}

std::string ShopifyOperationalGovernance::severityName(ShopifyGovernanceSeverity severity) {
    switch (severity) {
        case ShopifyGovernanceSeverity::info: return "info";
        case ShopifyGovernanceSeverity::warning: return "warning";
        case ShopifyGovernanceSeverity::critical: return "critical";
    }
    return "warning";
}

} // namespace elit21::shopify
