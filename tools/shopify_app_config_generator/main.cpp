#include "elit21/shopify/ShopifyApiVersionGuard.h"
#include "elit21/shopify/ShopifyAppConfigGenerator.h"

#include <filesystem>
#include <iostream>
#include <map>
#include <string>

namespace {

void usage() {
    std::cerr
        << "Usage:\n"
        << "  elit21_tool_shopify_app_config_generator <client_id> <application_https_url> "
           "<oauth_callback_https_url> <output_shopify.app.toml> [options]\n\n"
        << "Options:\n"
        << "  --api-version=YYYY-MM\n"
        << "  --scopes=scope1,scope2,...\n"
        << "  --webhook-uri=/webhooks/shopify|https://...\n"
        << "  --embedded=true|false\n"
        << "  --operational-webhooks=true|false\n";
}

bool parseBool(const std::string& value, bool& output) {
    if (value == "true" || value == "1" || value == "yes") { output = true; return true; }
    if (value == "false" || value == "0" || value == "no") { output = false; return true; }
    return false;
}

std::map<std::string, std::string> parseOptions(int argc, char** argv, int begin) {
    std::map<std::string, std::string> values;
    for (int index = begin; index < argc; ++index) {
        const std::string argument = argv[index];
        if (argument.rfind("--", 0) != 0) continue;
        const auto separator = argument.find('=');
        if (separator == std::string::npos) continue;
        values[argument.substr(2, separator - 2)] = argument.substr(separator + 1);
    }
    return values;
}

} // namespace

int main(int argc, char** argv) {
    if (argc < 5) {
        usage();
        return 2;
    }

    const auto options = parseOptions(argc, argv, 5);
    elit21::shopify::ShopifyAppConfigurationSpec spec;
    spec.client_id = argv[1];
    spec.application_url = argv[2];
    spec.redirect_urls = {argv[3]};
    spec.webhook_uri = options.contains("webhook-uri") ? options.at("webhook-uri") : "/webhooks/shopify";
    spec.api_version = options.contains("api-version")
        ? options.at("api-version")
        : elit21::shopify::ShopifyApiVersionGuard::currentStable();
    spec.scopes = options.contains("scopes")
        ? elit21::shopify::ShopifyScopes::parse(options.at("scopes"))
        : elit21::shopify::ShopifyScopes::autonomousManagerDefaults();

    if (options.contains("embedded") && !parseBool(options.at("embedded"), spec.embedded)) {
        std::cerr << "ERROR: --embedded expects true or false\n";
        return 3;
    }
    if (options.contains("operational-webhooks") &&
        !parseBool(options.at("operational-webhooks"), spec.include_operational_webhooks)) {
        std::cerr << "ERROR: --operational-webhooks expects true or false\n";
        return 3;
    }

    auto lifecycle = elit21::shopify::ShopifyApiVersionGuard::assess(spec.api_version);
    if (!lifecycle || !lifecycle.value().usable()) {
        std::cerr << "ERROR: selected Shopify API version is not a supported stable version: "
                  << spec.api_version << '\n';
        return 4;
    }

    const std::filesystem::path output_path = argv[4];
    auto result = elit21::shopify::ShopifyAppConfigGenerator::writeAtomically(
        spec, output_path.string());
    if (!result) {
        std::cerr << "ERROR: " << result.error() << '\n';
        return 5;
    }

    auto generated = elit21::shopify::ShopifyAppConfigGenerator::generateToml(spec);
    if (!generated) {
        std::cerr << "ERROR: generated file could not be revalidated: " << generated.error() << '\n';
        return 6;
    }
    std::cout << "Shopify app configuration written atomically\n"
              << "output=" << output_path.string() << '\n'
              << "api_version=" << spec.api_version << '\n'
              << "scopes=" << spec.scopes.commaSeparated() << '\n'
              << "operational_webhooks=" << (spec.include_operational_webhooks ? "true" : "false") << '\n'
              << "compliance_topics=" << elit21::shopify::ShopifyAppConfigGenerator::complianceTopics().size() << '\n';
    return 0;
}
