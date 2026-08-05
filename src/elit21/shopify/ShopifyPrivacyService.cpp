#include "elit21/shopify/ShopifyPrivacyService.h"

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
