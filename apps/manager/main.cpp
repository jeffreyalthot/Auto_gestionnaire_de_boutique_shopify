#include "elit21/app/Application.h"
#include "elit21/config/Config.h"

#include <atomic>
#include <chrono>
#include <csignal>
#include <cstdlib>
#include <filesystem>
#include <iostream>
#include <string>
#include <thread>

namespace {
std::atomic_bool interrupted{false};
void signalHandler(int) noexcept { interrupted.store(true); }
}

int main(int argc, char** argv) {
    std::signal(SIGINT, signalHandler);
    std::signal(SIGTERM, signalHandler);
    const char* environment = std::getenv("ELIT21_CONFIG");
    const std::string path = argc > 1 ? argv[1] : (environment ? environment : "config/app.json");
    if (!std::filesystem::exists(path)) {
        std::cerr << "Configuration introuvable: " << path << '\n';
        return 2;
    }
    auto loaded = elit21::Config::load(path);
    if (!loaded) {
        std::cerr << "Configuration invalide: " << loaded.error() << '\n';
        return 3;
    }
    auto config = loaded.take();
    std::cout << "ELIT21 Manager " ELIT21_VERSION " | "
              << (config.app.live_orders ? "LIVE" : "DRY-RUN")
              << " | Shopify=" << (config.shopify.shop.empty() ? "non configure" : config.shopify.shop)
              << '\n';
    elit21::Application application(std::move(config));
    auto initialized = application.initialize();
    if (!initialized) {
        std::cerr << "Initialisation impossible: " << initialized.error() << '\n';
        return 4;
    }
    std::atomic_bool finished{false};
    std::thread watcher([&] {
        while (!interrupted.load() && !finished.load()) {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
        if (interrupted.load()) application.stop();
    });
    const int result = application.run();
    finished.store(true);
    if (watcher.joinable()) watcher.join();
    return result;
}
