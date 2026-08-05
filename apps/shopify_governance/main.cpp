#include "elit21/config/Config.h"
#include "elit21/net/HttpClient.h"
#include "elit21/shopify/ShopifyClient.h"
#include "elit21/shopify/ShopifyOperationalGovernance.h"
#include "elit21/storage/Database.h"

#include <filesystem>
#include <iostream>

int main(int argc, char** argv) {
    const std::string config_path = argc > 1 ? argv[1] : "config/app.json";
    const bool force_live = argc > 2 && std::string(argv[2]) == "--live";
    auto config = elit21::Config::load(config_path);
    if (!config) { std::cerr << "ERROR: " << config.error() << '\n'; return 2; }

    elit21::Database database;
    if (auto parent = std::filesystem::path(config.value().app.database).parent_path(); !parent.empty()) {
        std::filesystem::create_directories(parent);
    }
    if (auto opened = database.open(config.value().app.database); !opened) {
        std::cerr << "ERROR: " << opened.error() << '\n'; return 3;
    }
    if (auto migrated = database.migrateDirectory(config.value().migrations_dir); !migrated) {
        std::cerr << "ERROR: " << migrated.error() << '\n'; return 4;
    }

    if (force_live) {
        elit21::HttpClient http;
        http.setTimeoutSeconds(config.value().network.request_timeout_seconds);
        http.setConnectTimeoutSeconds(config.value().network.connect_timeout_seconds);
        http.setTlsVerification(config.value().network.verify_tls);
        elit21::ShopifyClient client(config.value().shopify, http);
        auto report = elit21::shopify::ShopifyOperationalGovernance::evaluateLive(
            config.value(), client, database);
        if (!report) { std::cerr << "ERROR: " << report.error() << '\n'; return 5; }
        std::cout << report.value().toJson().dump(true) << '\n';
        return report.value().production_ready ? 0 : 6;
    }

    const auto report = elit21::shopify::ShopifyOperationalGovernance::evaluateOffline(
        config.value(), &database);
    std::cout << report.toJson().dump(true) << '\n';
    return report.production_ready || !config.value().app.live_orders ? 0 : 7;
}
