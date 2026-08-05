#include "elit21/config/Config.h"
#include "elit21/logging/Logger.h"
#include "elit21/net/HttpServer.h"
#include "elit21/shopify/ShopifyWebhookProcessor.h"
#include "elit21/storage/Database.h"

#include <atomic>
#include <chrono>
#include <csignal>
#include <filesystem>
#include <iostream>
#include <thread>

namespace {
std::atomic_bool stop_requested{false};
void onSignal(int) { stop_requested = true; }
}

int main(int argc, char** argv) {
    std::signal(SIGINT, onSignal);
    std::signal(SIGTERM, onSignal);
    const std::string config_path = argc > 1 ? argv[1] : "config/app.json";
    auto loaded = elit21::Config::load(config_path);
    if (!loaded) {
        std::cerr << loaded.error() << '\n';
        return 2;
    }
    auto config = loaded.take();
    std::filesystem::create_directories(config.app.data_dir);
    if (const auto parent = std::filesystem::path(config.app.database).parent_path(); !parent.empty()) {
        std::filesystem::create_directories(parent);
    }
    if (const auto parent = std::filesystem::path(config.app.log_file).parent_path(); !parent.empty()) {
        std::filesystem::create_directories(parent);
    }

    elit21::Logger logger(config.app.log_file);
    elit21::Database database;
    if (auto result = database.open(config.app.database); !result) {
        std::cerr << result.error() << '\n';
        return 3;
    }
    if (auto result = database.migrateDirectory(config.migrations_dir); !result) {
        std::cerr << result.error() << '\n';
        return 4;
    }

    elit21::RuntimeCounters counters;
    elit21::ShopifyWebhookProcessor processor(config.shopify, database, nullptr, &counters, &logger);
    elit21::HttpServer server;
    server.setMaximumRequestBytes(2U * 1024U * 1024U);
    auto started = server.start(config.shopify.webhook_port,
        [&](const elit21::IncomingRequest& request) -> elit21::OutgoingResponse {
            if (request.path == "/health" && request.method == "GET") {
                return {200, "application/json",
                    "{\"ok\":true,\"service\":\"webhook_server\",\"accepted\":" +
                    std::to_string(counters.accepted_events) + ",\"rejected\":" +
                    std::to_string(counters.rejected_events) + "}"};
            }
            return processor.handle(request);
        });
    if (!started) {
        std::cerr << started.error() << '\n';
        return 5;
    }

    logger.info("webhook", "Shopify webhook server listening on port " +
                std::to_string(config.shopify.webhook_port));
    std::cout << "Webhook server listening on port " << config.shopify.webhook_port << '\n';
    while (!stop_requested) std::this_thread::sleep_for(std::chrono::milliseconds(100));
    server.stop();
    database.metric("webhook.accepted", static_cast<double>(counters.accepted_events));
    database.metric("webhook.rejected", static_cast<double>(counters.rejected_events));
    return 0;
}
