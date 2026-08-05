#include "elit21/config/Config.h"
#include "elit21/json/Json.h"
#include "elit21/net/HttpClient.h"
#include "elit21/shopify/ShopifyClient.h"
#include "elit21/shopify/ShopifyProductSetSyncService.h"
#include "elit21/storage/Database.h"

#include <filesystem>
#include <iostream>
#include <string>

namespace {
elit21::Result<elit21::ShopifyProductCreate> parseProduct(const elit21::Json& input) {
    elit21::ShopifyProductCreate product;
    product.title = input.getString("title");
    product.description_html = input.getString("description_html");
    product.vendor = input.getString("vendor", "ELIT21");
    product.product_type = input.getString("product_type");
    product.source_product_id = input.getString("source_product_id");
    product.publish = input.getBool("publish", false);
    const auto variants = input.get("variants");
    for (std::size_t index = 0; index < variants.size(); ++index) {
        const auto node = variants.at(index);
        elit21::ShopifyProductVariantCreate variant;
        variant.option_name = node.getString("option_name", "Title");
        variant.option_value = node.getString("option_value");
        variant.sku = node.getString("sku");
        variant.source_product_id = product.source_product_id;
        variant.source_sku_id = node.getString("source_sku_id");
        variant.price_cad = node.getNumber("price_cad");
        variant.supplier_cost_cad = node.getNumber("supplier_cost_cad");
        variant.inventory = node.getInt("inventory");
        product.variants.push_back(std::move(variant));
    }
    if (!product.valid()) return elit21::Result<elit21::ShopifyProductCreate>::failure("Invalid product input");
    return elit21::Result<elit21::ShopifyProductCreate>::success(std::move(product));
}
}

int main(int argc, char** argv) {
    if (argc < 2) {
        std::cerr << "Usage: elit21_shopify_catalog_sync <product.json> [config/app.json] [--execute] [--sync] [--wait]\n";
        return 2;
    }
    const std::string config_path = argc > 2 && std::string(argv[2]).rfind("--", 0) != 0
        ? argv[2] : "config/app.json";
    bool execute = false, synchronous = false, wait = false;
    for (int index = 2; index < argc; ++index) {
        const std::string argument = argv[index];
        if (argument == "--execute") execute = true;
        if (argument == "--sync") synchronous = true;
        if (argument == "--wait") wait = true;
    }
    auto input = elit21::Json::parseFile(argv[1]);
    if (!input) { std::cerr << input.error() << '\n'; return 3; }
    auto product = parseProduct(input.value());
    if (!product) { std::cerr << product.error() << '\n'; return 4; }
    auto plan = elit21::shopify::ShopifyProductSetSyncService::plan(product.value(), synchronous);
    if (!plan) { std::cerr << plan.error() << '\n'; return 5; }
    if (!execute) {
        std::cout << plan.value().toJson().dump(true) << '\n';
        return 0;
    }
    auto config = elit21::Config::load(config_path);
    if (!config) { std::cerr << config.error() << '\n'; return 6; }
    if (config.value().app.dry_run || config.value().shopify.access_token.empty()) {
        std::cerr << "ERROR: --execute requires dry_run=false and a Shopify access token\n";
        return 7;
    }
    elit21::Database database;
    if (auto parent = std::filesystem::path(config.value().app.database).parent_path(); !parent.empty())
        std::filesystem::create_directories(parent);
    if (auto opened = database.open(config.value().app.database); !opened) { std::cerr << opened.error() << '\n'; return 8; }
    if (auto migrated = database.migrateDirectory(config.value().migrations_dir); !migrated) { std::cerr << migrated.error() << '\n'; return 9; }
    elit21::HttpClient http;
    http.setTlsVerification(config.value().network.verify_tls);
    http.setConnectTimeoutSeconds(config.value().network.connect_timeout_seconds);
    http.setTimeoutSeconds(config.value().network.request_timeout_seconds);
    elit21::ShopifyClient client(config.value().shopify, http);
    elit21::shopify::ShopifyProductSetSyncService service(client, database);
    auto result = service.execute(product.value(), synchronous);
    if (!result) { std::cerr << result.error() << '\n'; return 10; }
    if (wait && result.value().asynchronous && !result.value().operation_gid.empty()) {
        result = service.waitForCompletion(result.value().operation_gid);
        if (!result) { std::cerr << result.error() << '\n'; return 11; }
    }
    std::cout << result.value().toJson().dump(true) << '\n';
    return 0;
}
