#include "elit21/config/Config.h"
#include "elit21/net/HttpClient.h"
#include "elit21/shopify/ShopifyAutonomyEngine.h"
#include "elit21/shopify/ShopifyConfig.h"
#include "elit21/shopify/ShopifyServiceFactory.h"
#include "elit21/shopify/ShopifyWebhookRegistry.h"
#include "elit21/shopify/bulk/BulkProductExporter.h"
#include "elit21/storage/Database.h"

#include <filesystem>
#include <iostream>
#include <string>
#include <vector>

namespace {

struct Arguments {
    std::string config{"config/app.json"};
    std::string command{"validate"};
    bool live{false};
    int maximum_orders{250};
};

void printUsage() {
    std::cout
        << "ELIT21 Shopify Control Center v" ELIT21_VERSION "\n"
        << "Usage: elit21_shopify_control_center [config] [command] [options]\n\n"
        << "Commands:\n"
        << "  validate             Validate Shopify settings without network access\n"
        << "  health               Execute Shopify Admin API health check\n"
        << "  sync-orders          Ingest paid, unfulfilled orders into the local queue\n"
        << "  reconcile-webhooks   Reconcile required Shopify webhook subscriptions\n"
        << "  bulk-products        Start a product Bulk Operation\n"
        << "  report               Run the complete Shopify autonomy cycle\n\n"
        << "Options:\n"
        << "  --live                Permit mutations after all configuration checks\n"
        << "  --maximum-orders=N    Maximum orders for a synchronization pass\n";
}

Arguments parseArguments(int argc, char** argv) {
    Arguments value;
    if (argc > 1) value.config = argv[1];
    if (argc > 2) value.command = argv[2];
    for (int index = 3; index < argc; ++index) {
        const std::string argument = argv[index];
        if (argument == "--live") value.live = true;
        else if (argument.rfind("--maximum-orders=", 0) == 0) {
            try { value.maximum_orders = std::stoi(argument.substr(17)); }
            catch (...) { value.maximum_orders = 250; }
        }
    }
    value.maximum_orders = std::max(1, std::min(value.maximum_orders, 5000));
    return value;
}

void configureHttp(elit21::HttpClient& http, const elit21::NetworkConfig& network) {
    http.setTimeoutSeconds(network.request_timeout_seconds);
    http.setConnectTimeoutSeconds(network.connect_timeout_seconds);
    http.setMaximumResponseBytes(static_cast<std::size_t>(network.maximum_response_megabytes) * 1024U * 1024U);
    http.setTlsVerification(network.verify_tls);
    if (!network.ca_bundle.empty()) http.setCaBundle(network.ca_bundle);
    if (!network.proxy_url.empty()) http.setProxy(network.proxy_url);
}

int fail(const std::string& message, int code) {
    std::cerr << "ERROR: " << message << '\n';
    return code;
}

} // namespace

int main(int argc, char** argv) {
    const auto arguments = parseArguments(argc, argv);
    if (arguments.command == "help" || arguments.command == "--help") {
        printUsage();
        return 0;
    }

    auto loaded = elit21::Config::load(arguments.config);
    if (!loaded) return fail("Configuration invalide: " + loaded.error(), 2);
    auto config = loaded.take();

    elit21::shopify::ShopifyConfig validated(config.shopify);
    auto validation = validated.validate(arguments.command != "validate");
    if (!validation) return fail(validation.error(), 3);
    if (arguments.command == "validate") {
        std::cout << validated.sanitizedSummary().dump(true) << '\n';
        return 0;
    }
    if (arguments.live && (config.app.dry_run || !config.app.live_orders)) {
        return fail("Le mode --live exige app.dry_run=false et app.live_orders=true", 4);
    }

    elit21::HttpClient http;
    configureHttp(http, config.network);
    elit21::shopify::ShopifyServiceFactory factory(config.shopify, http);
    auto client_result = factory.createClient();
    if (!client_result) return fail(client_result.error(), 5);
    auto client = client_result.take();

    if (arguments.command == "health") {
        auto health = client->healthCheck();
        if (!health) return fail(health.error(), 6);
        std::cout << "{\"shopify\":\"healthy\"}\n";
        return 0;
    }

    elit21::Database database;
    auto opened = database.open(config.app.database);
    if (!opened) return fail(opened.error(), 7);
    auto migrated = database.migrateDirectory(config.migrations_dir);
    if (!migrated) return fail(migrated.error(), 8);

    const bool dry_run = !arguments.live;
    if (arguments.command == "reconcile-webhooks") {
        elit21::ShopifyWebhookRegistry registry(*client, config.shopify);
        auto report = registry.reconcile(dry_run);
        if (!report) return fail(report.error(), 9);
        std::cout << report.value().toJson().dump(true) << '\n';
        return report.value().failed == 0 ? 0 : 10;
    }
    if (arguments.command == "bulk-products") {
        elit21::shopify::bulk::BulkProductExporter exporter(*client);
        if (dry_run) {
            std::cout << "{\"planned\":true,\"operation\":\"bulk-products\"}\n";
            return 0;
        }
        auto operation = exporter.start();
        if (!operation) return fail(operation.error(), 11);
        std::cout << "{\"operation_gid\":\"" << operation.value().id
                  << "\",\"status\":\"" << operation.value().status << "\"}\n";
        return 0;
    }

    elit21::ShopifyAutonomyEngine engine(*client, database, config.shopify);
    if (arguments.command == "sync-orders" || arguments.command == "report") {
        const bool reconcile_webhooks = arguments.command == "report";
        auto report = engine.run(dry_run, arguments.maximum_orders, reconcile_webhooks);
        if (!report) return fail(report.error(), 12);
        std::cout << report.value().toJson().dump(true) << '\n';
        return report.value().healthy() ? 0 : 13;
    }

    printUsage();
    return fail("Commande Shopify inconnue: " + arguments.command, 14);
}
