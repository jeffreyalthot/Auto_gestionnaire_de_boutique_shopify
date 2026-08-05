#include "elit21/config/Config.h"

#include "elit21/shopify/ShopifyApiVersion.h"
#include "elit21/shopify/ShopifyEndpoints.h"
#include "elit21/util/StringUtil.h"

#include <algorithm>
#include <cstdlib>
#include <filesystem>
#include <regex>

namespace elit21 {
namespace {

std::string environment(const char* name, const std::string& fallback = {}) {
    const char* value = std::getenv(name);
    return value != nullptr ? std::string(value) : fallback;
}

bool environmentBool(const char* name, bool fallback) {
    const auto value = util::lower(environment(name));
    if (value.empty()) return fallback;
    if (value == "1" || value == "true" || value == "yes" || value == "on") return true;
    if (value == "0" || value == "false" || value == "no" || value == "off") return false;
    return fallback;
}

long environmentLong(const char* name, long fallback) {
    const auto value = environment(name);
    if (value.empty()) return fallback;
    try {
        return std::stol(value);
    } catch (...) {
        return fallback;
    }
}

bool isHttpsOrEmpty(const std::string& value) {
    return value.empty() || value.rfind("https://", 0) == 0;
}

std::string normalizedShop(std::string shop) {
    shop = util::trim(shop);
    constexpr const char* suffix = ".myshopify.com";
    if (shop.size() > std::char_traits<char>::length(suffix) &&
        shop.compare(shop.size() - std::char_traits<char>::length(suffix),
                     std::char_traits<char>::length(suffix), suffix) == 0) {
        shop.erase(shop.size() - std::char_traits<char>::length(suffix));
    }
    return util::lower(shop);
}

std::filesystem::path absoluteNormalized(const std::filesystem::path& value) {
    std::error_code error;
    auto absolute = std::filesystem::absolute(value, error);
    if (error) absolute = value;
    auto normalized = std::filesystem::weakly_canonical(absolute, error);
    return error ? absolute.lexically_normal() : normalized;
}

std::string resolveProjectPath(const std::filesystem::path& root, const std::string& value) {
    if (value.empty()) return {};
    const std::filesystem::path candidate(value);
    return absoluteNormalized(candidate.is_absolute() ? candidate : root / candidate).string();
}

} // namespace

std::string ShopifyConfig::graphqlEndpoint() const {
    return shopify::ShopifyEndpoints::adminGraphql(shop, api_version);
}

Result<Config> Config::load(const std::string& path) {
    const auto absolute_config = absoluteNormalized(path);
    auto parsed = Json::parseFile(absolute_config.string());
    if (!parsed) return Result<Config>::failure(parsed.error());
    const auto root = parsed.value();
    Config config;
    config.source_path = absolute_config.string();

    const auto application_node = root.get("application");
    std::filesystem::path inferred_root = absolute_config.parent_path();
    if (inferred_root.filename() == "config") inferred_root = inferred_root.parent_path();
    const auto configured_root = environment(
        "ELIT21_PROJECT_ROOT", application_node.getString("project_root"));
    if (!configured_root.empty()) {
        const std::filesystem::path candidate(configured_root);
        inferred_root = candidate.is_absolute()
            ? candidate
            : absolute_config.parent_path() / candidate;
    }
    inferred_root = absoluteNormalized(inferred_root);
    config.project_root = inferred_root.string();
    config.migrations_dir = resolveProjectPath(inferred_root, "migrations");

    const auto application = application_node;
    config.app.name = application.getString("name", "ELIT21 Shop Manager");
    config.app.data_dir = application.getString("data_dir", "data");
    config.app.log_file = application.getString("log_file", "logs/elit21.log");
    config.app.database = application.getString("database", "data/elit21.db");
    config.app.poll_seconds = application.getInt("poll_seconds", 60);
    config.app.worker_threads = application.getInt("worker_threads", 2);
    config.app.dry_run = application.getBool("dry_run", true);
    config.app.live_orders = application.getBool("live_orders", false);
    config.app.data_dir = resolveProjectPath(inferred_root, config.app.data_dir);
    config.app.log_file = resolveProjectPath(inferred_root, config.app.log_file);
    config.app.database = resolveProjectPath(inferred_root, config.app.database);

    const auto network = root.get("network");
    config.network.request_timeout_seconds = network.getInt("request_timeout_seconds", 45);
    config.network.connect_timeout_seconds = network.getInt("connect_timeout_seconds", 10);
    config.network.maximum_response_megabytes = network.getInt("maximum_response_megabytes", 16);
    config.network.verify_tls = network.getBool("verify_tls", true);
    config.network.ca_bundle = network.getString("ca_bundle");
    config.network.proxy_url = network.getString("proxy_url");
    if (!config.network.ca_bundle.empty()) {
        config.network.ca_bundle = resolveProjectPath(inferred_root, config.network.ca_bundle);
    }

    const auto shopify = root.get("shopify");
    config.shopify.shop = normalizedShop(shopify.getString("shop"));
    config.shopify.access_token = shopify.getString("access_token");
    config.shopify.api_version = shopify.getString("api_version", "2026-07");
    config.shopify.webhook_secret = shopify.getString("webhook_secret");
    config.shopify.webhook_base_url = shopify.getString("webhook_base_url");
    config.shopify.webhook_port = shopify.getInt("webhook_port", 8080);

    const auto aliexpress = root.get("aliexpress");
    config.aliexpress.app_key = aliexpress.getString("app_key");
    config.aliexpress.app_secret = aliexpress.getString("app_secret");
    config.aliexpress.access_token = aliexpress.getString("access_token");
    config.aliexpress.gateway = aliexpress.getString("gateway", "https://eco.taobao.com/router/rest");
    config.aliexpress.iop_gateway = aliexpress.getString("iop_gateway", "https://api-sg.aliexpress.com/rest");
    config.aliexpress.oauth_url = aliexpress.getString("oauth_url", "https://api-sg.aliexpress.com/oauth/authorize");
    config.aliexpress.country = aliexpress.getString("country", "CA");
    config.aliexpress.currency = aliexpress.getString("currency", "CAD");
    config.aliexpress.language = aliexpress.getString("language", "EN");

    const auto pricing = root.get("pricing");
    config.pricing.markup_percent_before_shipping = pricing.getNumber("markup_percent_before_shipping", 100);
    config.pricing.round_to_cents = pricing.getBool("round_to_cents", true);
    config.pricing.minimum_price_cad = pricing.getNumber("minimum_price_cad", 5);
    config.pricing.maximum_price_cad = pricing.getNumber("maximum_price_cad", 5000);

    const auto sourcing = root.get("sourcing");
    config.sourcing.minimum_seller_score = sourcing.getNumber("minimum_seller_score", 90);
    config.sourcing.minimum_product_rating = sourcing.getNumber("minimum_product_rating", 4.2);
    config.sourcing.minimum_orders = sourcing.getInt("minimum_orders", 20);
    config.sourcing.maximum_delivery_days = sourcing.getInt("maximum_delivery_days", 45);
    config.sourcing.auto_publish = sourcing.getBool("auto_publish", false);

    const auto inventory = root.get("inventory");
    config.inventory.safety_buffer = inventory.getInt("safety_buffer", 2);
    config.inventory.out_of_stock_quantity = inventory.getInt("out_of_stock_quantity", 0);
    config.inventory.sync_batch_size = inventory.getInt("sync_batch_size", 50);

    const auto risk = root.get("risk");
    config.risk.maximum_order_value_cad = risk.getNumber("maximum_order_value_cad", 750);
    config.risk.require_valid_canadian_postal_code = risk.getBool("require_valid_canadian_postal_code", true);
    config.risk.block_po_boxes = risk.getBool("block_po_boxes", false);

    const auto automation = root.get("automation");
    config.automation.catalog_sync = automation.getBool("catalog_sync", true);
    config.automation.inventory_sync = automation.getBool("inventory_sync", true);
    config.automation.price_sync = automation.getBool("price_sync", true);
    config.automation.order_processing = automation.getBool("order_processing", true);
    config.automation.tracking_sync = automation.getBool("tracking_sync", true);
    config.automation.reports = automation.getBool("reports", true);

    const auto terminal = root.get("terminal");
    config.terminal.refresh_ms = terminal.getInt("refresh_ms", 500);
    config.terminal.fixed_dashboard = terminal.getBool("fixed_dashboard", true);
    config.terminal.event_lines = terminal.getInt("event_lines", 8);

    config.shopify.shop = normalizedShop(environment("SHOPIFY_SHOP", config.shopify.shop));
    config.shopify.access_token = environment("SHOPIFY_ACCESS_TOKEN", config.shopify.access_token);
    config.shopify.api_version = environment("SHOPIFY_API_VERSION", config.shopify.api_version);
    config.shopify.webhook_secret = environment("SHOPIFY_WEBHOOK_SECRET", config.shopify.webhook_secret);
    config.aliexpress.app_key = environment("ALIEXPRESS_APP_KEY", config.aliexpress.app_key);
    config.aliexpress.app_secret = environment("ALIEXPRESS_APP_SECRET", config.aliexpress.app_secret);
    config.aliexpress.access_token = environment("ALIEXPRESS_ACCESS_TOKEN", config.aliexpress.access_token);
    config.app.live_orders = environmentBool("ELIT21_LIVE_ORDERS", config.app.live_orders);
    config.app.dry_run = !config.app.live_orders;
    config.network.verify_tls = environmentBool("ELIT21_VERIFY_TLS", config.network.verify_tls);
    config.network.request_timeout_seconds = environmentLong("ELIT21_HTTP_TIMEOUT", config.network.request_timeout_seconds);
    config.network.proxy_url = environment("HTTPS_PROXY", config.network.proxy_url);

    const auto validation = config.validate();
    if (!validation) return Result<Config>::failure(validation.error());
    return Result<Config>::success(std::move(config));
}

Result<void> Config::validate() const {
    if (source_path.empty() || project_root.empty() || migrations_dir.empty()) {
        return Result<void>::failure("Chemins runtime du projet non initialisés.");
    }
    if (app.name.empty()) return Result<void>::failure("Nom d'application manquant.");
    if (app.poll_seconds < 5 || app.poll_seconds > 86400) {
        return Result<void>::failure("poll_seconds doit être compris entre 5 et 86400.");
    }
    if (app.worker_threads < 1 || app.worker_threads > 64) {
        return Result<void>::failure("worker_threads doit être compris entre 1 et 64.");
    }
    if (network.request_timeout_seconds < 1 || network.request_timeout_seconds > 600 ||
        network.connect_timeout_seconds < 1 || network.connect_timeout_seconds > network.request_timeout_seconds) {
        return Result<void>::failure("Délais réseau invalides.");
    }
    if (network.maximum_response_megabytes < 1 || network.maximum_response_megabytes > 256) {
        return Result<void>::failure("maximum_response_megabytes doit être compris entre 1 et 256.");
    }
    if (pricing.markup_percent_before_shipping < 100.0) {
        return Result<void>::failure("La marge avant livraison ne peut pas être inférieure à 100%.");
    }
    if (pricing.minimum_price_cad < 0 || pricing.maximum_price_cad <= pricing.minimum_price_cad) {
        return Result<void>::failure("Bornes de prix invalides.");
    }
    if (aliexpress.country != "CA") return Result<void>::failure("Le pays cible AliExpress doit être CA.");
    if (aliexpress.currency != "CAD") return Result<void>::failure("La devise doit être CAD.");
    if (!isHttpsOrEmpty(aliexpress.gateway) || !isHttpsOrEmpty(aliexpress.iop_gateway) ||
        !isHttpsOrEmpty(aliexpress.oauth_url)) {
        return Result<void>::failure("Les URL AliExpress doivent utiliser HTTPS.");
    }
    if (!shopify.shop.empty() && !std::regex_match(shopify.shop, std::regex("^[a-z0-9][a-z0-9-]{1,61}[a-z0-9]$"))) {
        return Result<void>::failure("Sous-domaine Shopify invalide.");
    }
    if (!shopify::ShopifyApiVersion::parse(shopify.api_version)) {
        return Result<void>::failure("Version Shopify invalide: " + shopify.api_version);
    }
    if (shopify.webhook_port < 1 || shopify.webhook_port > 65535) {
        return Result<void>::failure("Port webhook invalide.");
    }
    if (!isHttpsOrEmpty(shopify.webhook_base_url)) {
        return Result<void>::failure("webhook_base_url doit utiliser HTTPS.");
    }
    if (inventory.safety_buffer < 0 || inventory.sync_batch_size < 1 || inventory.sync_batch_size > 250) {
        return Result<void>::failure("Configuration d'inventaire invalide.");
    }
    if (sourcing.maximum_delivery_days < 1 || sourcing.maximum_delivery_days > 180) {
        return Result<void>::failure("maximum_delivery_days invalide.");
    }
    if (risk.maximum_order_value_cad <= 0) {
        return Result<void>::failure("maximum_order_value_cad doit être positif.");
    }
    if (terminal.refresh_ms < 100 || terminal.event_lines < 1 || terminal.event_lines > 100) {
        return Result<void>::failure("Configuration du terminal invalide.");
    }
    if (app.live_orders) {
        if (shopify.shop.empty() || shopify.access_token.empty() || shopify.webhook_secret.empty()) {
            return Result<void>::failure("Le mode réel exige la boutique, le jeton et le secret webhook Shopify.");
        }
        if (aliexpress.app_key.empty() || aliexpress.app_secret.empty() || aliexpress.access_token.empty()) {
            return Result<void>::failure("Le mode réel exige les identifiants et le jeton AliExpress.");
        }
        if (!network.verify_tls) {
            return Result<void>::failure("La vérification TLS ne peut pas être désactivée en mode réel.");
        }
    }
    return Result<void>::success();
}

Json Config::sanitizedSummary() const {
    Json root = Json::object();
    root.set("name", app.name);
    root.set("dry_run", app.dry_run);
    root.set("live_orders", app.live_orders);
    root.set("shopify_shop", shopify.shop);
    root.set("shopify_api_version", shopify.api_version);
    root.set("shopify_configured", !shopify.shop.empty() && !shopify.access_token.empty());
    root.set("aliexpress_configured", !aliexpress.app_key.empty() && !aliexpress.access_token.empty());
    root.set("country", aliexpress.country);
    root.set("currency", aliexpress.currency);
    root.set("markup_percent_before_shipping", pricing.markup_percent_before_shipping);
    root.set("verify_tls", network.verify_tls);
    root.set("poll_seconds", app.poll_seconds);
    root.set("project_root", project_root);
    root.set("migrations_dir", migrations_dir);
    root.set("database", app.database);
    return root;
}

} // namespace elit21
