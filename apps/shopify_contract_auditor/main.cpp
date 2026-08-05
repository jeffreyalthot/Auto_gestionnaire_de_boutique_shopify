#include "elit21/config/Config.h"
#include "elit21/shopify/ShopifyApiContractAuditor.h"
#include "elit21/storage/Database.h"

#include <filesystem>
#include <fstream>
#include <iostream>

int main(int argc, char** argv) {
    const std::filesystem::path root = argc > 1 ? argv[1] : ".";
    const std::string config_path = argc > 2 ? argv[2] : (root / "config/app.json").string();
    const std::filesystem::path output = argc > 3
        ? argv[3]
        : root / "SHOPIFY_API_CONTRACT_AUDIT_V9.md";

    auto config = elit21::Config::load(config_path);
    if (!config) { std::cerr << "ERROR: " << config.error() << '\n'; return 2; }
    elit21::Database database;
    if (auto parent = std::filesystem::path(config.value().app.database).parent_path(); !parent.empty()) {
        std::filesystem::create_directories(parent);
    }
    if (auto opened = database.open(config.value().app.database); !opened) {
        std::cerr << "ERROR: " << opened.error() << '\n'; return 3;
    }
    if (auto migrated = database.migrateDirectory((root / "migrations").string()); !migrated) {
        std::cerr << "ERROR: " << migrated.error() << '\n'; return 4;
    }
    auto report = elit21::shopify::ShopifyApiContractAuditor::audit(root, &database);
    if (!report) { std::cerr << "ERROR: " << report.error() << '\n'; return 5; }
    std::ofstream file(output, std::ios::binary | std::ios::trunc);
    file << report.value().markdown();
    std::cout << report.value().toJson().dump(true) << '\n';
    return report.value().passed ? 0 : 6;
}
