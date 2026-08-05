#include "elit21/config/Config.h"
#include "elit21/json/Json.h"
#include "elit21/shopify/ShopifyWebhookReliabilityMonitor.h"
#include "elit21/storage/Database.h"

#include <filesystem>
#include <fstream>
#include <iostream>

int main(int argc, char** argv) {
    if (argc < 2) {
        std::cerr << "Usage: elit21_shopify_webhook_slo <deliveries.json> [config/app.json] [report.md]\n";
        return 2;
    }
    const std::string config_path = argc > 2 ? argv[2] : "config/app.json";
    const std::filesystem::path report_path = argc > 3 ? argv[3] : "SHOPIFY_WEBHOOK_SLO_V9.md";
    auto input = elit21::Json::parseFile(argv[1]);
    if (!input) { std::cerr << input.error() << '\n'; return 3; }
    auto samples = elit21::shopify::ShopifyWebhookReliabilityMonitor::parseSamples(input.value());
    if (!samples) { std::cerr << samples.error() << '\n'; return 4; }
    auto config = elit21::Config::load(config_path);
    if (!config) { std::cerr << config.error() << '\n'; return 5; }
    elit21::Database database;
    if (auto parent = std::filesystem::path(config.value().app.database).parent_path(); !parent.empty()) {
        std::filesystem::create_directories(parent);
    }
    if (auto opened = database.open(config.value().app.database); !opened) {
        std::cerr << opened.error() << '\n'; return 6;
    }
    if (auto migrated = database.migrateDirectory(config.value().migrations_dir); !migrated) {
        std::cerr << migrated.error() << '\n'; return 7;
    }
    auto report = elit21::shopify::ShopifyWebhookReliabilityMonitor::analyzeAndPersist(
        samples.value(), database);
    if (!report) { std::cerr << report.error() << '\n'; return 8; }
    std::ofstream output(report_path, std::ios::binary | std::ios::trunc);
    output << report.value().markdown();
    std::cout << report.value().toJson().dump(true) << '\n';
    return report.value().healthy ? 0 : 9;
}
