#include "elit21/app/Application.h"

#include "elit21/analytics/Analytics.h"
#include "elit21/shopify/ShopifyWebhookRegistry.h"

#include <filesystem>

namespace elit21 {

Application::Application(Config config)
    : config_(std::move(config)),
      logger_(config_.app.log_file),
      compliance_(CanadaCompliance::loadKeywords("resources/prohibited_keywords.txt")),
      risk_(config_.risk),
      pricing_(config_.pricing),
      shopify_(config_.shopify, http_),
      aliexpress_(config_.aliexpress, http_),
      sourcing_(config_.sourcing, compliance_, risk_),
      catalog_(aliexpress_, shopify_, pricing_, sourcing_, db_, logger_, config_, counters_),
      inventory_(aliexpress_, shopify_, pricing_, db_, logger_, config_, counters_),
      orders_(shopify_, aliexpress_, compliance_, risk_, db_, logger_, config_, counters_),
      fulfillment_(aliexpress_, shopify_, db_, logger_, counters_),
      dashboard_(config_.terminal, counters_, logger_),
      automation_(config_, catalog_, inventory_, orders_, fulfillment_, reports_, dashboard_, counters_),
      webhook_processor_(config_.shopify, db_, &orders_, &counters_, &logger_) {}

Result<void> Application::initialize() {
    std::filesystem::create_directories(config_.app.data_dir);
    if (const auto parent = std::filesystem::path(config_.app.log_file).parent_path(); !parent.empty()) {
        std::filesystem::create_directories(parent);
    }
    if (const auto parent = std::filesystem::path(config_.app.database).parent_path(); !parent.empty()) {
        std::filesystem::create_directories(parent);
    }

    http_.setTimeoutSeconds(config_.network.request_timeout_seconds);
    http_.setConnectTimeoutSeconds(config_.network.connect_timeout_seconds);
    http_.setMaximumResponseBytes(
        static_cast<std::size_t>(config_.network.maximum_response_megabytes) * 1024U * 1024U);
    http_.setTlsVerification(config_.network.verify_tls);
    http_.setCaBundle(config_.network.ca_bundle);
    http_.setProxy(config_.network.proxy_url);
    http_.setUserAgent("ELIT21-Shop-Manager/4.0");

    auto opened = db_.open(config_.app.database);
    if (!opened) return opened;
    auto migrated = db_.migrateDirectory(config_.migrations_dir);
    if (!migrated) return migrated;
    auto recovered = db_.recoverStaleTasks(900);
    if (!recovered) return Result<void>::failure(recovered.error());
    db_.setRuntimeState("configuration", config_.sanitizedSummary().dump());
    db_.audit("INFO", "startup", "Database initialized", config_.sanitizedSummary().dump());
    logger_.info("startup", "Database initialized with checksummed migrations.");
    if (recovered.value() > 0) {
        logger_.warning("startup", "Recovered stale tasks: " + std::to_string(recovered.value()));
    }

    const bool shopify_ready = !config_.shopify.access_token.empty() && !config_.shopify.shop.empty();
    if (shopify_ready) {
        auto health = shopify_.healthCheck();
        if (!health) {
            logger_.warning("startup", "Shopify: " + health.error());
        } else {
            logger_.info("startup", "Shopify connected.");
            if (!config_.shopify.webhook_base_url.empty()) {
                ShopifyWebhookRegistry registry(shopify_, config_.shopify);
                auto subscriptions = registry.reconcile(config_.app.dry_run);
                if (!subscriptions) {
                    logger_.warning("startup", "Shopify webhooks: " + subscriptions.error());
                } else {
                    db_.setRuntimeState("shopify_webhook_registry", subscriptions.value().toJson().dump());
                    logger_.info(
                        "startup",
                        "Shopify webhooks existing=" + std::to_string(subscriptions.value().existing) +
                        " created=" + std::to_string(subscriptions.value().created) +
                        " planned=" + std::to_string(subscriptions.value().planned));
                }
            }
        }
    }
    if (!config_.aliexpress.app_key.empty() && !config_.aliexpress.app_secret.empty()) {
        auto health = aliexpress_.healthCheck();
        if (!health) logger_.warning("startup", "AliExpress: " + health.error());
        else logger_.info("startup", "AliExpress connected.");
    }

    auto server = server_.start(config_.shopify.webhook_port,
                                [this](const IncomingRequest& request) { return handleHttp(request); });
    if (!server) return server;
    logger_.info("startup", "Webhook server listening on port " +
                            std::to_string(config_.shopify.webhook_port));
    return Result<void>::success();
}

OutgoingResponse Application::handleHttp(const IncomingRequest& request) {
    if (request.path == "/health" && request.method == "GET") {
        Analytics analytics;
        Json health = analytics.snapshot(counters_);
        health.set("database", db_.isOpen());
        health.set("webhook_server", server_.running());
        health.set("configuration", config_.sanitizedSummary());
        const auto metrics = shopify_.apiMetrics();
        Json shopify_metrics = Json::object();
        shopify_metrics.set("requests", static_cast<std::int64_t>(metrics.requests));
        shopify_metrics.set("retries", static_cast<std::int64_t>(metrics.retries));
        shopify_metrics.set("throttles", static_cast<std::int64_t>(metrics.throttles));
        shopify_metrics.set("transient_failures", static_cast<std::int64_t>(metrics.transient_failures));
        shopify_metrics.set("permanent_failures", static_cast<std::int64_t>(metrics.permanent_failures));
        shopify_metrics.set("last_http_status", static_cast<std::int64_t>(metrics.last_http_status));
        shopify_metrics.set("last_elapsed_seconds", metrics.last_elapsed_seconds);
        shopify_metrics.set("currently_available_cost", metrics.currently_available_cost);
        health.set("shopify_graphql", shopify_metrics);
        return {200, "application/json", health.dump()};
    }

    if (request.path.rfind("/webhooks/shopify", 0) == 0) {
        return webhook_processor_.handle(request);
    }
    return {404, "application/json", "{\"error\":\"not_found\"}"};
}

int Application::run() {
    dashboard_.start();
    automation_.run();
    dashboard_.stop();
    server_.stop();
    return 0;
}

void Application::stop() {
    automation_.requestStop();
}

} // namespace elit21
