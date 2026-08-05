#include "elit21/config/Config.h"
#include "elit21/json/Json.h"
#include "elit21/storage/Database.h"
#include "elit21/util/TimeUtil.h"

#include <array>
#include <iostream>
#include <string>

namespace {
struct ScheduleEntry {
    const char* kind;
    int priority;
    bool enabled;
};
} // namespace

int main(int argc, char** argv) {
    const std::string config_path = argc > 1 ? argv[1] : "config/app.json";
    auto config = elit21::Config::load(config_path);
    if (!config) {
        std::cerr << config.error() << '\n';
        return 2;
    }

    elit21::Database database;
    if (auto result = database.open(config.value().app.database); !result) {
        std::cerr << result.error() << '\n';
        return 3;
    }
    if (auto result = database.migrateDirectory(config.value().migrations_dir); !result) {
        std::cerr << result.error() << '\n';
        return 4;
    }

    const std::array<ScheduleEntry, 5> entries{{
        {"catalog_sync", 50, config.value().automation.catalog_sync},
        {"inventory_sync", 60, config.value().automation.inventory_sync || config.value().automation.price_sync},
        {"order_poll", 20, config.value().automation.order_processing},
        {"tracking_sync", 40, config.value().automation.tracking_sync},
        {"daily_report", 90, config.value().automation.reports},
    }};

    int inserted = 0;
    int deduplicated = 0;
    for (const auto& entry : entries) {
        if (!entry.enabled) {
            std::cout << entry.kind << " disabled\n";
            continue;
        }
        elit21::Json payload = elit21::Json::object();
        payload.set("scheduled", true);
        payload.set("scheduled_at", elit21::util::utcNowIso());
        auto task = database.enqueueUniqueTask(
            entry.kind,
            payload.dump(),
            entry.priority,
            {},
            std::string("scheduled:") + entry.kind);
        if (!task) {
            std::cerr << task.error() << '\n';
            return 5;
        }
        if (task.value().inserted) {
            ++inserted;
            std::cout << entry.kind << " task_id=" << task.value().id << " inserted\n";
        } else {
            ++deduplicated;
            std::cout << entry.kind << " task_id=" << task.value().id << " already_pending\n";
        }
    }
    database.audit(
        "INFO",
        "scheduler",
        "Scheduled autonomous tasks",
        "{\"inserted\":" + std::to_string(inserted) + ",\"deduplicated\":" +
            std::to_string(deduplicated) + "}");
    std::cout << "inserted=" << inserted << " deduplicated=" << deduplicated << '\n';
    return 0;
}
