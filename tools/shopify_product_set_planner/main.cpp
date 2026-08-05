#include "elit21/json/Json.h"
#include "elit21/shopify/ShopifyProductSetSyncService.h"

#include <fstream>
#include <iostream>
#include <sstream>

int main(int argc, char** argv) {
    if (argc < 2) {
        std::cerr << "Usage: elit21_tool_shopify_product_set_planner <product.json> [--sync]\n";
        return 2;
    }
    auto input = elit21::Json::parseFile(argv[1]);
    if (!input) { std::cerr << input.error() << '\n'; return 3; }
    elit21::ShopifyProductCreate product;
    product.title = input.value().getString("title");
    product.description_html = input.value().getString("description_html");
    product.vendor = input.value().getString("vendor", "ELIT21");
    product.product_type = input.value().getString("product_type");
    product.source_product_id = input.value().getString("source_product_id");
    product.publish = input.value().getBool("publish", false);
    const auto variants = input.value().get("variants");
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
    const bool synchronous = argc > 2 && std::string(argv[2]) == "--sync";
    auto plan = elit21::shopify::ShopifyProductSetSyncService::plan(product, synchronous);
    if (!plan) { std::cerr << "ERROR: " << plan.error() << '\n'; return 4; }
    std::cout << plan.value().toJson().dump(true) << '\n';
    return 0;
}
