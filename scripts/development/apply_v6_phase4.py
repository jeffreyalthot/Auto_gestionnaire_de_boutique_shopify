from pathlib import Path
root=Path('/mnt/data/ELIT21_Shopify_AliExpress_Autonomous_Manager-v6.0.0-SHOPIFY-TRANSACTIONAL')

def write(rel, text):
    p=root/rel; p.parent.mkdir(parents=True, exist_ok=True); p.write_text(text, encoding='utf-8')

def replace(rel, old, new, count=-1):
    p=root/rel; s=p.read_text(encoding='utf-8')
    if old not in s: raise SystemExit(f'pattern missing in {rel}: {old[:120]!r}')
    p.write_text(s.replace(old,new,count), encoding='utf-8')

# Allow manual-review orders without supplier mapping to be durably retained.
replace('src/elit21/storage/Database.cpp', '''        if (line.aliexpress_product_id.empty() || line.aliexpress_sku_id.empty())
            return Result<bool>::failure("Shopify order line has no AliExpress mapping");
''', '')

# Database completion API for compliance request lifecycle.
replace('include/elit21/storage/Database.h',
'''    Result<void> recordShopifyPrivacyRequest(const ShopifyPrivacyRequestRecord& request);
''',
'''    Result<void> recordShopifyPrivacyRequest(const ShopifyPrivacyRequestRecord& request);
    Result<void> completeShopifyPrivacyRequest(const std::string& request_id,
                                               const std::string& status,
                                               const std::string& result_path = {},
                                               const std::string& last_error = {});
''')

marker='''Result<void> Database::setRuntimeState(const std::string& key, const std::string& value_json) {'''
method=r'''Result<void> Database::completeShopifyPrivacyRequest(const std::string& request_id,
                                                            const std::string& status,
                                                            const std::string& result_path,
                                                            const std::string& last_error) {
    if (request_id.empty() || status.empty()) {
        return Result<void>::failure("Privacy request id and status are required");
    }
    return prepareAndStep(
        "UPDATE shopify_privacy_requests SET status=?,result_path=?,last_error=?,"
        "completed_at=CASE WHEN ? IN ('completed','redacted','failed') THEN CURRENT_TIMESTAMP ELSE completed_at END,"
        "updated_at=CURRENT_TIMESTAMP WHERE request_id=?",
        [&](sqlite3_stmt* statement) {
            sqlite3_bind_text(statement, 1, status.c_str(), -1, SQLITE_TRANSIENT);
            sqlite3_bind_text(statement, 2, result_path.c_str(), -1, SQLITE_TRANSIENT);
            sqlite3_bind_text(statement, 3, last_error.c_str(), -1, SQLITE_TRANSIENT);
            sqlite3_bind_text(statement, 4, status.c_str(), -1, SQLITE_TRANSIENT);
            sqlite3_bind_text(statement, 5, request_id.c_str(), -1, SQLITE_TRANSIENT);
        });
}

'''
replace('src/elit21/storage/Database.cpp', marker, method+marker)

write('include/elit21/shopify/ShopifyPrivacyService.h', r'''#pragma once

#include "elit21/core/Result.h"
#include "elit21/json/Json.h"
#include "elit21/storage/Database.h"

#include <string>
#include <vector>

namespace elit21::shopify {

struct ShopifyPrivacyResult {
    std::string request_id;
    std::string topic;
    std::string status;
    std::string export_path;
    int affected_orders{0};
    bool duplicate{false};

    [[nodiscard]] Json toJson() const;
};

class ShopifyPrivacyService {
public:
    ShopifyPrivacyService(Database& database, std::string data_directory);

    Result<ShopifyPrivacyResult> processTask(const std::string& task_kind,
                                             const Json& task_payload);

private:
    struct ParsedRequest {
        std::string request_id;
        std::string topic;
        std::string shop_domain;
        std::string customer_id;
        std::string payload_hash;
        std::vector<std::string> order_ids;
        Json body;
    };

    Result<ParsedRequest> parseRequest(const std::string& task_kind,
                                       const Json& task_payload) const;
    Result<ShopifyPrivacyResult> exportCustomerData(const ParsedRequest& request);
    Result<ShopifyPrivacyResult> redactCustomerData(const ParsedRequest& request);
    Result<ShopifyPrivacyResult> redactShopData(const ParsedRequest& request);
    static std::vector<std::string> parseOrderIds(const Json& body);
    static std::string atomicWrite(const std::string& directory,
                                   const std::string& file_name,
                                   const std::string& body);

    Database& database_;
    std::string data_directory_;
};

} // namespace elit21::shopify
''')

write('src/elit21/shopify/ShopifyPrivacyService.cpp', r'''#include "elit21/shopify/ShopifyPrivacyService.h"

#include "elit21/security/Crypto.h"
#include "elit21/util/StringUtil.h"
#include "elit21/util/TimeUtil.h"

#include <filesystem>
#include <fstream>
#include <system_error>

namespace elit21::shopify {
namespace {
std::string scalarId(const Json& object, const std::string& key) {
    return object.getScalarString(key);
}

std::string safeFilePart(std::string value) {
    for (char& character : value) {
        const bool ok = (character >= 'a' && character <= 'z') ||
                        (character >= 'A' && character <= 'Z') ||
                        (character >= '0' && character <= '9') || character == '-' || character == '_';
        if (!ok) character = '_';
    }
    if (value.empty()) value = crypto::randomHex(8);
    if (value.size() > 96) value.resize(96);
    return value;
}
} // namespace

Json ShopifyPrivacyResult::toJson() const {
    Json output = Json::object();
    output.set("request_id", request_id);
    output.set("topic", topic);
    output.set("status", status);
    output.set("export_path", export_path);
    output.set("affected_orders", affected_orders);
    output.set("duplicate", duplicate);
    return output;
}

ShopifyPrivacyService::ShopifyPrivacyService(Database& database, std::string data_directory)
    : database_(database), data_directory_(std::move(data_directory)) {}

std::vector<std::string> ShopifyPrivacyService::parseOrderIds(const Json& body) {
    std::vector<std::string> output;
    const auto append = [&](const Json& array, std::vector<std::string>& destination) {
        if (!array.isArray()) return;
        for (std::size_t index = 0; index < array.size(); ++index) {
            auto id = array.at(index).scalarStringValue();
            if (!id.empty()) destination.push_back(std::move(id));
        }
    };
    append(body.get("orders_requested"), output);
    append(body.get("orders_to_redact"), output);
    return output;
}

Result<ShopifyPrivacyService::ParsedRequest> ShopifyPrivacyService::parseRequest(
    const std::string& task_kind, const Json& task_payload) const {
    const auto raw_payload = task_payload.getString("payload");
    if (raw_payload.empty()) return Result<ParsedRequest>::failure("Privacy task payload is missing");
    auto parsed = Json::parse(raw_payload);
    if (!parsed || !parsed.value().isObject()) {
        return Result<ParsedRequest>::failure("Privacy webhook body is not a JSON object");
    }

    ParsedRequest request;
    request.body = parsed.take();
    request.topic = task_payload.getString("topic");
    if (request.topic.empty()) {
        if (task_kind == "shopify_customers_data_request") request.topic = "customers/data_request";
        else if (task_kind == "shopify_customers_redact") request.topic = "customers/redact";
        else if (task_kind == "shopify_shop_redact") request.topic = "shop/redact";
    }
    request.shop_domain = util::lower(util::trim(request.body.getString("shop_domain")));
    request.customer_id = scalarId(request.body.get("customer"), "id");
    request.order_ids = parseOrderIds(request.body);
    request.payload_hash = task_payload.getString("payload_hash", crypto::sha256Hex(raw_payload));
    request.request_id = task_payload.getString("webhook_id");
    const auto data_request_id = scalarId(request.body.get("data_request"), "id");
    if (!data_request_id.empty()) request.request_id = data_request_id;

    if (request.request_id.empty() || request.topic.empty() || request.shop_domain.empty()) {
        return Result<ParsedRequest>::failure("Privacy webhook identity is incomplete");
    }
    return Result<ParsedRequest>::success(std::move(request));
}

std::string ShopifyPrivacyService::atomicWrite(const std::string& directory,
                                               const std::string& file_name,
                                               const std::string& body) {
    const auto target_directory = std::filesystem::path(directory) / "privacy_exports";
    std::filesystem::create_directories(target_directory);
    const auto target = target_directory / file_name;
    const auto temporary = target.string() + ".tmp-" + crypto::randomHex(6);
    {
        std::ofstream stream(temporary, std::ios::binary | std::ios::trunc);
        if (!stream) return {};
        stream.write(body.data(), static_cast<std::streamsize>(body.size()));
        stream.flush();
        if (!stream) { std::error_code error; std::filesystem::remove(temporary, error); return {}; }
    }
    std::error_code error;
    std::filesystem::rename(temporary, target, error);
    if (error) {
        std::filesystem::remove(target, error);
        error.clear();
        std::filesystem::rename(temporary, target, error);
    }
    if (error) { std::filesystem::remove(temporary, error); return {}; }
    std::filesystem::permissions(target,
        std::filesystem::perms::owner_read | std::filesystem::perms::owner_write,
        std::filesystem::perm_options::replace, error);
    return target.string();
}

Result<ShopifyPrivacyResult> ShopifyPrivacyService::exportCustomerData(const ParsedRequest& request) {
    auto orders = database_.ordersByShopifyIds(request.order_ids);
    if (!orders) return Result<ShopifyPrivacyResult>::failure(orders.error());

    Json document = Json::object();
    document.set("request_id", request.request_id);
    document.set("topic", request.topic);
    document.set("shop_domain", request.shop_domain);
    document.set("customer_id", request.customer_id);
    document.set("generated_at", util::utcNowIso());
    Json order_array = Json::array();
    for (const auto& order : orders.value()) {
        Json item = Json::object();
        item.set("shopify_order_id", order.shopify_id);
        item.set("customer_email", order.customer_email);
        item.set("currency", order.currency);
        item.set("total_cad", order.total);
        item.set("status", order.status);
        auto lines = database_.orderLines(order.shopify_id);
        Json line_array = Json::array();
        if (lines) {
            for (const auto& line : lines.value()) {
                Json value = Json::object();
                value.set("shopify_line_item_id", line.shopify_line_id);
                value.set("sku", line.sku);
                value.set("title", line.title);
                value.set("quantity", line.quantity);
                value.set("unit_price_cad", line.unit_price_cad);
                line_array.push(value);
            }
        }
        item.set("line_items", line_array);
        order_array.push(item);
    }
    document.set("orders", order_array);

    const auto path = atomicWrite(data_directory_,
        "shopify-data-request-" + safeFilePart(request.request_id) + ".json",
        document.dump(true));
    if (path.empty()) return Result<ShopifyPrivacyResult>::failure("Unable to write privacy export atomically");

    auto completed = database_.completeShopifyPrivacyRequest(request.request_id, "completed", path);
    if (!completed) return Result<ShopifyPrivacyResult>::failure(completed.error());
    ShopifyPrivacyResult result{request.request_id, request.topic, "completed", path,
                                static_cast<int>(orders.value().size()), false};
    database_.audit("INFO", "shopify.privacy", "Customer data export generated",
                    result.toJson().dump(), request.request_id);
    return Result<ShopifyPrivacyResult>::success(std::move(result));
}

Result<ShopifyPrivacyResult> ShopifyPrivacyService::redactCustomerData(const ParsedRequest& request) {
    auto redacted = database_.redactShopifyOrders(request.order_ids, "Shopify customers/redact request");
    if (!redacted) return Result<ShopifyPrivacyResult>::failure(redacted.error());
    auto completed = database_.completeShopifyPrivacyRequest(request.request_id, "redacted");
    if (!completed) return Result<ShopifyPrivacyResult>::failure(completed.error());
    ShopifyPrivacyResult result{request.request_id, request.topic, "redacted", {}, redacted.value(), false};
    database_.audit("WARNING", "shopify.privacy", "Customer order data redacted",
                    result.toJson().dump(), request.request_id);
    return Result<ShopifyPrivacyResult>::success(std::move(result));
}

Result<ShopifyPrivacyResult> ShopifyPrivacyService::redactShopData(const ParsedRequest& request) {
    auto redacted = database_.redactAllShopifyCustomerData("Shopify shop/redact request");
    if (!redacted) return Result<ShopifyPrivacyResult>::failure(redacted.error());
    Json state = Json::object();
    state.set("shop_domain", request.shop_domain);
    state.set("redacted", true);
    state.set("redacted_at", util::utcNowIso());
    auto stored = database_.setRuntimeState("shopify.shop_redacted", state.dump());
    if (!stored) return Result<ShopifyPrivacyResult>::failure(stored.error());
    auto completed = database_.completeShopifyPrivacyRequest(request.request_id, "redacted");
    if (!completed) return Result<ShopifyPrivacyResult>::failure(completed.error());
    ShopifyPrivacyResult result{request.request_id, request.topic, "redacted", {}, redacted.value(), false};
    database_.audit("WARNING", "shopify.privacy", "All retained Shopify customer data redacted",
                    result.toJson().dump(), request.request_id);
    return Result<ShopifyPrivacyResult>::success(std::move(result));
}

Result<ShopifyPrivacyResult> ShopifyPrivacyService::processTask(const std::string& task_kind,
                                                                const Json& task_payload) {
    auto parsed = parseRequest(task_kind, task_payload);
    if (!parsed) return Result<ShopifyPrivacyResult>::failure(parsed.error());
    const auto& request = parsed.value();

    Json ids = Json::array();
    for (const auto& id : request.order_ids) { Json value = Json::object(); value.set("id", id); ids.push(value); }
    ShopifyPrivacyRequestRecord record;
    record.request_id = request.request_id;
    record.topic = request.topic;
    record.shop_domain = request.shop_domain;
    record.customer_id = request.customer_id;
    record.orders_json = ids.dump();
    record.payload_hash = request.payload_hash;
    auto recorded = database_.recordShopifyPrivacyRequest(record);
    if (!recorded) return Result<ShopifyPrivacyResult>::failure(recorded.error());

    Result<ShopifyPrivacyResult> result = Result<ShopifyPrivacyResult>::failure("Unsupported privacy task");
    if (task_kind == "shopify_customers_data_request") result = exportCustomerData(request);
    else if (task_kind == "shopify_customers_redact") result = redactCustomerData(request);
    else if (task_kind == "shopify_shop_redact") result = redactShopData(request);
    if (!result) database_.completeShopifyPrivacyRequest(request.request_id, "failed", {}, result.error());
    return result;
}

} // namespace elit21::shopify
''')

write('include/elit21/shopify/ShopifyProductionReadiness.h', r'''#pragma once

#include "elit21/config/Config.h"
#include "elit21/json/Json.h"
#include "elit21/storage/Database.h"

#include <string>
#include <vector>

namespace elit21::shopify {

struct ShopifyReadinessCheck {
    std::string id;
    std::string level;
    std::string message;
    bool passed{false};
    int weight{0};
    [[nodiscard]] Json toJson() const;
};

struct ShopifyReadinessReport {
    int score{0};
    bool production_ready{false};
    std::vector<ShopifyReadinessCheck> checks;
    [[nodiscard]] Json toJson() const;
};

class ShopifyProductionReadiness {
public:
    static ShopifyReadinessReport evaluate(const Config& config, const Database* database = nullptr);
};

} // namespace elit21::shopify
''')

write('src/elit21/shopify/ShopifyProductionReadiness.cpp', r'''#include "elit21/shopify/ShopifyProductionReadiness.h"

#include "elit21/shopify/ShopifyApiVersion.h"
#include "elit21/shopify/ShopifyEndpoints.h"

#include <algorithm>
#include <array>
#include <set>

namespace elit21::shopify {
namespace {
void add(ShopifyReadinessReport& report, std::string id, std::string level,
         std::string message, bool passed, int weight) {
    report.checks.push_back({std::move(id), std::move(level), std::move(message), passed, weight});
}
} // namespace

Json ShopifyReadinessCheck::toJson() const {
    Json output = Json::object();
    output.set("id", id); output.set("level", level); output.set("message", message);
    output.set("passed", passed); output.set("weight", weight); return output;
}

Json ShopifyReadinessReport::toJson() const {
    Json output = Json::object(); output.set("score", score); output.set("production_ready", production_ready);
    Json values = Json::array(); for (const auto& check : checks) values.push(check.toJson());
    output.set("checks", values); return output;
}

ShopifyReadinessReport ShopifyProductionReadiness::evaluate(const Config& config, const Database* database) {
    ShopifyReadinessReport report;
    const auto valid_shop = ShopifyEndpoints::validateShop(config.shopify.shop);
    add(report, "shop_domain", "critical", "Canonical myshopify.com shop identifier", bool(valid_shop), 10);
    const auto version = ShopifyApiVersion::parse(config.shopify.api_version);
    add(report, "api_version", "critical", "Quarterly Shopify Admin API version is syntactically valid", bool(version), 8);
    add(report, "admin_token", "critical", "Admin API access token is configured", config.shopify.access_token.size() >= 16, 12);
    add(report, "webhook_secret", "critical", "Webhook signing secret has sufficient entropy", config.shopify.webhook_secret.size() >= 24, 12);
    add(report, "webhook_https", "critical", "Webhook public base URL uses HTTPS",
        ShopifyEndpoints::isHttpsUrl(config.shopify.webhook_base_url), 10);
    add(report, "tls", "critical", "TLS certificate verification is enabled", config.network.verify_tls, 10);
    add(report, "currency", "critical", "Store integration is locked to CAD", config.aliexpress.currency == "CAD", 6);
    add(report, "country", "critical", "Supplier destination is locked to Canada", config.aliexpress.country == "CA", 6);
    add(report, "markup", "critical", "Markup is at least 100% before shipping",
        config.pricing.markup_percent_before_shipping >= 100.0, 10);
    add(report, "database", "critical", "Transactional SQLite database is open", database && database->isOpen(), 8);
    add(report, "live_mode", "warning", "Live mode and dry-run flags are mutually consistent",
        config.app.live_orders != config.app.dry_run, 4);
    add(report, "aliexpress_credentials", "critical", "AliExpress application and access token are configured",
        !config.aliexpress.app_key.empty() && !config.aliexpress.app_secret.empty() &&
        !config.aliexpress.access_token.empty(), 4);

    int passed_weight = 0, total_weight = 0; bool critical_failure = false;
    for (const auto& check : report.checks) {
        total_weight += check.weight;
        if (check.passed) passed_weight += check.weight;
        else if (check.level == "critical") critical_failure = true;
    }
    report.score = total_weight == 0 ? 0 : (passed_weight * 100 + total_weight / 2) / total_weight;
    report.production_ready = !critical_failure && report.score >= 90;
    return report;
}

} // namespace elit21::shopify
''')

write('apps/shopify_readiness/main.cpp', r'''#include "elit21/config/Config.h"
#include "elit21/shopify/ShopifyProductionReadiness.h"
#include "elit21/storage/Database.h"

#include <filesystem>
#include <iostream>

int main(int argc, char** argv) {
    const std::string config_path = argc > 1 ? argv[1] : "config/app.json";
    auto config = elit21::Config::load(config_path);
    if (!config) { std::cerr << config.error() << '\n'; return 2; }
    elit21::Database database;
    if (auto parent = std::filesystem::path(config.value().app.database).parent_path(); !parent.empty())
        std::filesystem::create_directories(parent);
    auto opened = database.open(config.value().app.database);
    if (!opened) { std::cerr << opened.error() << '\n'; return 3; }
    auto migrated = database.migrateDirectory("migrations");
    if (!migrated) { std::cerr << migrated.error() << '\n'; return 4; }
    const auto report = elit21::shopify::ShopifyProductionReadiness::evaluate(config.value(), &database);
    std::cout << report.toJson().dump(true) << '\n';
    return report.production_ready ? 0 : (config.value().app.live_orders ? 5 : 0);
}
''')

# Wire privacy service into worker.
replace('apps/worker_service/main.cpp',
'''#include "elit21/shopify/ShopifyClient.h"
#include "elit21/shopify/ShopifyWebhook.h"
''',
'''#include "elit21/shopify/ShopifyClient.h"
#include "elit21/shopify/ShopifyPrivacyService.h"
#include "elit21/shopify/ShopifyWebhook.h"
''')
replace('apps/worker_service/main.cpp',
'''          fulfillment(aliexpress, shopify, database, logger, counters),
          webhook(config.shopify) {}
''',
'''          fulfillment(aliexpress, shopify, database, logger, counters),
          privacy(database, config.app.data_dir),
          webhook(config.shopify) {}
''')
replace('apps/worker_service/main.cpp',
'''        if (task.kind == "shopify_webhook_unknown") {
            return database.audit("WARNING", "shopify", "Unknown Shopify webhook topic retained", payload.dump());
        }
        if (task.kind == "place_supplier_order") {
''',
'''        if (task.kind == "shopify_customers_data_request" ||
            task.kind == "shopify_customers_redact" || task.kind == "shopify_shop_redact") {
            auto processed = privacy.processTask(task.kind, payload);
            return processed ? elit21::Result<void>::success()
                             : elit21::Result<void>::failure(processed.error());
        }
        if (task.kind == "review_shopify_order") {
            const auto order_id = payload.getString("shopify_order_id");
            if (order_id.empty()) return elit21::Result<void>::failure("Review task has no Shopify order id");
            return database.audit("WARNING", "shopify.order.review",
                                  "Shopify order remains in manual review", payload.dump(), order_id);
        }
        if (task.kind == "shopify_webhook_unknown") {
            return database.audit("WARNING", "shopify", "Unknown Shopify webhook topic retained", payload.dump());
        }
        if (task.kind == "place_supplier_order") {
''')
replace('apps/worker_service/main.cpp',
'''    elit21::FulfillmentManager fulfillment;
    elit21::ReportGenerator reports;
    elit21::ShopifyWebhook webhook;
''',
'''    elit21::FulfillmentManager fulfillment;
    elit21::ReportGenerator reports;
    elit21::shopify::ShopifyPrivacyService privacy;
    elit21::ShopifyWebhook webhook;
''')

# Serialize autonomy runs with a durable lease.
replace('src/elit21/shopify/ShopifyAutonomyEngine.cpp',
'''#include "elit21/security/Crypto.h"
#include "elit21/shopify/ShopifyEndpoints.h"
''',
'''#include "elit21/security/Crypto.h"
#include "elit21/shopify/ShopifyEndpoints.h"
#include <utility>
''')
old='''    ShopifyAutonomyReport report;
    report.dry_run = dry_run;
    auto validation = validateConfiguration(!dry_run);
'''
new='''    ShopifyAutonomyReport report;
    report.dry_run = dry_run;
    const std::string lease_owner = "shopify-autonomy-" + crypto::randomHex(8);
    auto lease = database_.acquireLease("shopify.autonomy", lease_owner, 900);
    if (!lease) return Result<ShopifyAutonomyReport>::failure(lease.error());
    if (!lease.value()) return Result<ShopifyAutonomyReport>::failure("Another Shopify autonomy cycle owns the lease");
    struct LeaseRelease {
        Database& database; std::string owner;
        ~LeaseRelease() { database.releaseLease("shopify.autonomy", owner); }
    } lease_release{database_, lease_owner};
    auto validation = validateConfiguration(!dry_run);
'''
replace('src/elit21/shopify/ShopifyAutonomyEngine.cpp', old, new)

# Keep migration number monotonic and add compliance metadata fields.
write('migrations/0035_shopify_privacy_completion_indexes.sql', r'''CREATE INDEX IF NOT EXISTS idx_shopify_privacy_requests_shop_topic
ON shopify_privacy_requests(shop_domain,topic,received_at);

CREATE INDEX IF NOT EXISTS idx_orders_customer_email
ON orders(customer_email) WHERE customer_email IS NOT NULL AND customer_email<>'';

CREATE INDEX IF NOT EXISTS idx_shopify_order_events_transition
ON shopify_order_events(from_status,to_status,created_at);
''')

# Copy reproducibility scripts into project.
for phase in range(1,5):
    source=Path(f'/mnt/data/apply_v6_phase{phase}.py')
    if source.exists():
        write(f'scripts/development/apply_v6_phase{phase}.py', source.read_text(encoding='utf-8'))

print('phase4 applied')
