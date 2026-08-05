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
std::atomic_bool stop_requested{false};
void onSignal(int) noexcept { stop_requested.store(true); }

struct Options {
    std::string config_path{"config/app.json"};
    bool validate_only{false};
    bool print_config{false};
};

void usage() {
    std::cout << "Usage: elit21_autonomous_manager [--config PATH] [--validate] [--print-config]\n";
}

bool parseOptions(int argc, char** argv, Options& options) {
    if (const char* environment = std::getenv("ELIT21_CONFIG")) options.config_path = environment;
    for (int index = 1; index < argc; ++index) {
        const std::string argument = argv[index];
        if (argument == "--config" && index + 1 < argc) options.config_path = argv[++index];
        else if (argument == "--validate") options.validate_only = true;
        else if (argument == "--print-config") options.print_config = true;
        else if (argument == "--help" || argument == "-h") { usage(); return false; }
        else if (!argument.empty() && argument.front() != '-') options.config_path = argument;
        else { std::cerr << "Option inconnue: " << argument << '\n'; usage(); return false; }
    }
    return true;
}
} // namespace

int main(int argc, char** argv) {
    Options options;
    if (!parseOptions(argc, argv, options)) return argc > 1 ? 2 : 0;
    std::signal(SIGINT, onSignal);
    std::signal(SIGTERM, onSignal);

    if (!std::filesystem::exists(options.config_path)) {
        std::cerr << "Configuration introuvable: " << options.config_path << '\n';
        return 2;
    }
    auto loaded = elit21::Config::load(options.config_path);
    if (!loaded) {
        std::cerr << "Configuration invalide: " << loaded.error() << '\n';
        return 3;
    }
    auto config = loaded.take();
    if (options.print_config || options.validate_only) {
        std::cout << config.sanitizedSummary().dump(true) << '\n';
    }
    if (options.validate_only) {
        std::cout << "VALID\n";
        return 0;
    }

    std::cout << "ELIT21 Autonomous Manager " ELIT21_VERSION "\n"
              << "Mode: " << (config.app.live_orders ? "LIVE" : "DRY-RUN") << '\n'
              << "Configuration: " << options.config_path << '\n';
    elit21::Application application(std::move(config));
    if (auto initialized = application.initialize(); !initialized) {
        std::cerr << "Initialisation impossible: " << initialized.error() << '\n';
        return 4;
    }

    std::atomic_bool finished{false};
    std::thread watcher([&] {
        while (!stop_requested.load() && !finished.load()) {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
        if (stop_requested.load()) application.stop();
    });

    int result = 5;
    try {
        result = application.run();
    } catch (const std::exception& error) {
        std::cerr << "Erreur fatale: " << error.what() << '\n';
    } catch (...) {
        std::cerr << "Erreur fatale inconnue\n";
    }
    finished.store(true);
    if (watcher.joinable()) watcher.join();
    return result;
}
