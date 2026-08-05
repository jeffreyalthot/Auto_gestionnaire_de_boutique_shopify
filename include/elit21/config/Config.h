#pragma once

#include "elit21/core/Result.h"
#include "elit21/json/Json.h"

#include <string>

namespace elit21 {

struct AppConfig {
    std::string name, data_dir, log_file, database;
    int poll_seconds{60}, worker_threads{2};
    bool dry_run{true}, live_orders{false};
};

struct NetworkConfig {
    long request_timeout_seconds{45};
    long connect_timeout_seconds{10};
    int maximum_response_megabytes{16};
    bool verify_tls{true};
    std::string ca_bundle;
    std::string proxy_url;
};

struct ShopifyConfig {
    std::string shop, access_token, api_version{"2026-07"}, webhook_secret, webhook_base_url;
    int webhook_port{8080};
    std::string graphqlEndpoint() const;
};

struct AliExpressConfig {
    std::string app_key, app_secret, access_token, gateway, iop_gateway, oauth_url;
    std::string country{"CA"}, currency{"CAD"}, language{"EN"};
};

struct PricingConfig {
    double markup_percent_before_shipping{100.0}, minimum_price_cad{5.0}, maximum_price_cad{5000.0};
    bool round_to_cents{true};
};

struct SourcingConfig {
    double minimum_seller_score{90.0}, minimum_product_rating{4.2};
    int minimum_orders{20}, maximum_delivery_days{45};
    bool auto_publish{false};
};

struct InventoryConfig {
    int safety_buffer{2}, out_of_stock_quantity{0}, sync_batch_size{50};
};

struct RiskConfig {
    double maximum_order_value_cad{750.0};
    bool require_valid_canadian_postal_code{true}, block_po_boxes{false};
};

struct AutomationConfig {
    bool catalog_sync{true}, inventory_sync{true}, price_sync{true}, order_processing{true};
    bool tracking_sync{true}, reports{true};
};

struct TerminalConfig {
    int refresh_ms{500}, event_lines{8};
    bool fixed_dashboard{true};
};

struct Config {
    std::string source_path, project_root, migrations_dir;
    AppConfig app;
    NetworkConfig network;
    ShopifyConfig shopify;
    AliExpressConfig aliexpress;
    PricingConfig pricing;
    SourcingConfig sourcing;
    InventoryConfig inventory;
    RiskConfig risk;
    AutomationConfig automation;
    TerminalConfig terminal;

    static Result<Config> load(const std::string& path);
    Result<void> validate() const;
    Json sanitizedSummary() const;
};

} // namespace elit21
