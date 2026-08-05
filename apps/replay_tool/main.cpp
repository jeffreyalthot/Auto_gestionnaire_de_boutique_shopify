#include "elit21/config/Config.h"
#include "elit21/net/HttpServer.h"
#include "elit21/security/Crypto.h"
#include "elit21/shopify/ShopifyWebhookProcessor.h"
#include "elit21/storage/Database.h"

#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

int main(int argc, char** argv) {
    if (argc < 4) {
        std::cerr << "usage: replay_tool <config> <topic> <payload.json> [webhook-id]\n";
        return 2;
    }
    auto loaded = elit21::Config::load(argv[1]);
    if (!loaded) { std::cerr << loaded.error() << '\n'; return 3; }
    std::ifstream input(argv[3], std::ios::binary);
    if (!input) { std::cerr << "Payload introuvable\n"; return 4; }
    std::ostringstream buffer; buffer << input.rdbuf();
    auto config = loaded.take();
    if (config.shopify.webhook_secret.empty()) {
        std::cerr << "webhook_secret requis pour une reprise fidele\n";
        return 5;
    }
    const auto parent = std::filesystem::path(config.app.database).parent_path();
    if (!parent.empty()) std::filesystem::create_directories(parent);
    elit21::Database database;
    if (auto opened = database.open(config.app.database); !opened) { std::cerr << opened.error() << '\n'; return 6; }
    if (auto migrated = database.migrateDirectory(config.migrations_dir); !migrated) { std::cerr << migrated.error() << '\n'; return 7; }
    elit21::ShopifyWebhookProcessor processor(config.shopify, database);
    elit21::IncomingRequest request;
    request.method = "POST";
    request.path = "/webhooks/shopify";
    request.body = buffer.str();
    request.headers["x-shopify-topic"] = argv[2];
    request.headers["x-shopify-shop-domain"] = config.shopify.shop + ".myshopify.com";
    request.headers["x-shopify-api-version"] = config.shopify.api_version;
    request.headers["x-shopify-webhook-id"] = argc > 4 ? argv[4] : "replay-" + elit21::crypto::sha256Hex(request.body).substr(0, 24);
    request.headers["x-shopify-hmac-sha256"] = elit21::crypto::hmacSha256Base64(config.shopify.webhook_secret, request.body);
    const auto response = processor.handle(request);
    std::cout << "status=" << response.status << '\n' << response.body << '\n';
    return response.status >= 200 && response.status < 300 ? 0 : 8;
}
