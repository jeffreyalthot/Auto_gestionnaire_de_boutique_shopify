#include "elit21/config/Config.h"
#include "elit21/json/Json.h"
#include "elit21/shopify/ShopifyApiVersion.h"
#include "elit21/shopify/ShopifyEndpoints.h"

#include <filesystem>
#include <iostream>
#include <string>
#include <vector>

int main(int argc, char** argv) {
    const std::string path = argc > 1 ? argv[1] : "config/app.json";
    const bool production_check = argc > 2 && std::string(argv[2]) == "--production";
    std::vector<std::string> warnings;
    std::vector<std::string> errors;

    if (!std::filesystem::exists(path)) {
        std::cerr << "INVALID: configuration introuvable: " << path << '\n';
        return 2;
    }
    auto loaded = elit21::Config::load(path);
    if (!loaded) {
        std::cerr << "INVALID: " << loaded.error() << '\n';
        return 3;
    }
    const auto& config = loaded.value();
    if (!elit21::shopify::ShopifyApiVersion::parse(config.shopify.api_version))
        errors.emplace_back("Version API Shopify invalide");
    if (!config.shopify.shop.empty() && !elit21::shopify::ShopifyEndpoints::validateShop(config.shopify.shop))
        errors.emplace_back("Nom de boutique Shopify invalide");
    if (!config.shopify.webhook_base_url.empty() &&
        config.shopify.webhook_base_url.rfind("https://", 0) != 0)
        warnings.emplace_back("Les webhooks de production exigent une URL HTTPS");
    if (config.app.live_orders && config.app.dry_run)
        errors.emplace_back("live_orders et dry_run ne peuvent pas etre vrais simultanement");
    if (config.app.live_orders && config.shopify.access_token.empty())
        errors.emplace_back("Jeton Shopify requis pour live_orders");
    if (config.app.live_orders && (config.aliexpress.app_key.empty() || config.aliexpress.app_secret.empty()))
        errors.emplace_back("Identifiants AliExpress requis pour live_orders");
    if (!config.network.verify_tls)
        warnings.emplace_back("La verification TLS est desactivee");
    if (config.pricing.markup_percent_before_shipping < 100.0)
        errors.emplace_back("La majoration avant livraison doit rester au moins a 100 %");
    if (config.app.worker_threads < 1)
        errors.emplace_back("worker_threads doit etre positif");
    const auto database_parent = std::filesystem::path(config.app.database).parent_path();
    if (!database_parent.empty() && !std::filesystem::exists(database_parent))
        warnings.emplace_back("Le dossier parent de la base sera cree au demarrage");
    if (production_check && (config.app.dry_run || !config.app.live_orders))
        errors.emplace_back("Le controle --production exige dry_run=false et live_orders=true");

    elit21::Json report = elit21::Json::object();
    report.set("valid", errors.empty());
    report.set("production_check", production_check);
    report.set("configuration", config.sanitizedSummary());
    elit21::Json error_array = elit21::Json::array();
    for (const auto& error : errors) error_array.push(elit21::Json(json_object_new_string(error.c_str())));
    elit21::Json warning_array = elit21::Json::array();
    for (const auto& warning : warnings) warning_array.push(elit21::Json(json_object_new_string(warning.c_str())));
    report.set("errors", error_array);
    report.set("warnings", warning_array);
    std::cout << report.dump(true) << '\n';
    std::cout << (errors.empty() ? "VALID\n" : "INVALID\n");
    return errors.empty() ? 0 : 4;
}
