#include "elit21/config/Config.h"
#include "elit21/shopify/ShopifyProductionReadiness.h"
#include "elit21/storage/Database.h"

#include <filesystem>
#include <iostream>

int main(int argc, char** argv) {
    const std::string config_path = argc > 1 ? argv[1] : "config/app.json";
    auto config = elit21::Config::load(config_path);
    if (!config) { std::cerr << config.error() << '\n'; return 2; }
    elit21::Database database;
    if (auto parent = std::filesystem::path(config.value().app.database).parent_path(); !parent.empty())
        std::filesystem::create_directories(parent);
    auto opened = database.open(config.value().app.database);
    if (!opened) { std::cerr << opened.error() << '\n'; return 3; }
    auto migrated = database.migrateDirectory(config.value().migrations_dir);
    if (!migrated) { std::cerr << migrated.error() << '\n'; return 4; }
    const auto report = elit21::shopify::ShopifyProductionReadiness::evaluate(config.value(), &database);
    std::cout << report.toJson().dump(true) << '\n';
    return report.production_ready ? 0 : (config.value().app.live_orders ? 5 : 0);
}
