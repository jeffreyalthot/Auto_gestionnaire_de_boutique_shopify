#include "elit21/config/Config.h"
#include "elit21/shopify/ShopifyEndpoints.h"
#include "elit21/shopify/ShopifyReconciliationCoordinator.h"
#include "elit21/storage/Database.h"

#include <filesystem>
#include <iostream>

int main(int argc, char** argv) {
    const std::string config_path = argc > 1 ? argv[1] : "config/app.json";
    auto config = elit21::Config::load(config_path);
    if (!config) {
        std::cerr << config.error() << '\n';
        return 2;
    }
    if (const auto parent =
            std::filesystem::path(config.value().app.database).parent_path();
        !parent.empty()) {
        std::filesystem::create_directories(parent);
    }
    elit21::Database database;
    auto opened = database.open(config.value().app.database);
    if (!opened) {
        std::cerr << opened.error() << '\n';
        return 3;
    }
    auto migrated = database.migrateDirectory(config.value().migrations_dir);
    if (!migrated) {
        std::cerr << migrated.error() << '\n';
        return 4;
    }
    const auto shop_domain =
        elit21::shopify::ShopifyEndpoints::normalizeShop(config.value().shopify.shop) +
        ".myshopify.com";
    elit21::shopify::ShopifyReconciliationCoordinator coordinator(
        database, shop_domain);
    auto scheduled = coordinator.schedulePeriodic(
        config.value().automation.catalog_sync,
        config.value().automation.inventory_sync ||
            config.value().automation.price_sync,
        config.value().automation.order_processing,
        config.value().automation.tracking_sync,
        true);
    if (!scheduled) {
        std::cerr << scheduled.error() << '\n';
        return 5;
    }
    std::cout << scheduled.value().toJson().dump(true) << '\n';
    return 0;
}
