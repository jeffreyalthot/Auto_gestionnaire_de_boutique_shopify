#include "elit21/shopify/ShopifyWebhookProcessor.h"

#include "elit21/json/Json.h"
#include "elit21/security/Crypto.h"
#include "elit21/shopify/webhooks/ShopifyWebhookTopic.h"
#include "elit21/shopify/ShopifyApiVersionGuard.h"
#include "elit21/shopify/ShopifyTimestamp.h"
#include "elit21/util/StringUtil.h"

#include <utility>

namespace elit21 {

ShopifyWebhookProcessor::ShopifyWebhookProcessor(ShopifyConfig config,
                                                 Database& database,
                                                 OrderManager* order_manager,
                                                 RuntimeCounters* counters,
                                                 Logger* logger)
    : config_(std::move(config)),
      database_(database),
      order_manager_(order_manager),
      counters_(counters),
      logger_(logger),
      webhook_(config_) {}

std::string ShopifyWebhookProcessor::taskKindForTopic(const std::string& topic) {
    return shopify::webhooks::taskKind(shopify::webhooks::parseTopic(topic));
}

bool ShopifyWebhookProcessor::shopDomainAccepted(const IncomingRequest& request) const {
    const auto iterator = request.headers.find("x-shopify-shop-domain");
    if (iterator == request.headers.end() || config_.shop.empty()) return true;
    const auto expected = util::lower(config_.shop + ".myshopify.com");
    return util::lower(util::trim(iterator->second)) == expected;
}

std::string ShopifyWebhookProcessor::topicFor(const IncomingRequest& request) const {
    const auto iterator = request.headers.find("x-shopify-topic");
    if (iterator != request.headers.end()) return util::lower(util::trim(iterator->second));
    if (request.path.find("orders-paid") != std::string::npos) return "orders/paid";
    return "unknown";
}

OutgoingResponse ShopifyWebhookProcessor::handle(const IncomingRequest& request) {
    if (request.method != "POST" ||
        (request.path != "/webhooks/shopify" && request.path != "/webhooks/shopify/orders-paid")) {
        return {404, "application/json", R"({"error":"not_found"})"};
    }
    if (request.body.empty() || request.body.size() > 2U * 1024U * 1024U) {
        if (counters_) ++counters_->rejected_events;
        return {413, "application/json", R"({"error":"invalid_body_size"})"};
    }
    const auto content_type = request.headers.find("content-type");
    if (content_type != request.headers.end() &&
        util::lower(content_type->second).find("application/json") == std::string::npos) {
        if (counters_) ++counters_->rejected_events;
        return {415, "application/json", R"({"error":"json_required"})"};
    }
    if (!shopDomainAccepted(request)) {
        if (counters_) ++counters_->rejected_events;
        database_.audit("WARNING", "webhook", "Shopify shop domain mismatch");
        return {403, "application/json", R"({"error":"shop_domain_mismatch"})"};
    }
    if (!webhook_.verify(request)) {
        if (counters_) ++counters_->rejected_events;
        database_.audit("WARNING", "webhook", "Invalid Shopify HMAC");
        if (logger_) logger_->warning("webhook", "Invalid Shopify HMAC rejected");
        return {401, "application/json", R"({"error":"invalid_hmac"})"};
    }

    const auto id_iterator = request.headers.find("x-shopify-webhook-id");
    const std::string webhook_id = id_iterator != request.headers.end()
        ? util::trim(id_iterator->second) : crypto::sha256Hex(request.body);
    const std::string topic = topicFor(request);
    const auto parsed_topic = shopify::webhooks::parseTopic(topic);
    const std::string payload_hash = crypto::sha256Hex(request.body);

    const auto version_header = request.headers.find("x-shopify-api-version");
    if (version_header != request.headers.end()) {
        const auto served_version = util::trim(version_header->second);
        auto assessment = shopify::ShopifyApiVersionGuard::assess(
            config_.api_version, served_version);
        if (assessment) {
            database_.setRuntimeState(
                "shopify.api_version." + config_.shop,
                assessment.value().toJson().dump());
            if (!assessment.value().version_matches) {
                database_.audit("WARNING", "webhook",
                    "Shopify webhook API version differs from configured version",
                    assessment.value().toJson().dump(), webhook_id);
            }
        } else {
            database_.audit("WARNING", "webhook",
                "Shopify webhook API version could not be assessed",
                Json::object().dump(), webhook_id);
        }
    }

    auto recorded = database_.recordWebhook("shopify", webhook_id, topic, payload_hash);
    if (!recorded) return {500, "application/json", R"({"error":"webhook_storage_failed"})"};
    if (!recorded.value()) {
        if (counters_) ++counters_->accepted_events;
        return {200, "application/json", R"({"accepted":true,"duplicate":true})"};
    }

    auto generic_payload = Json::parse(request.body);
    if (!generic_payload || !generic_payload.value().isObject()) {
        database_.markWebhookProcessed("shopify", webhook_id, "invalid_json_payload");
        if (counters_) ++counters_->rejected_events;
        return {400, "application/json", R"({"error":"invalid_json"})"};
    }
    const auto event_header = request.headers.find("x-shopify-event-id");
    const std::string event_id = event_header == request.headers.end()
        ? std::string{} : util::trim(event_header->second);
    const auto triggered_header = request.headers.find("x-shopify-triggered-at");
    const std::string raw_triggered_at = triggered_header == request.headers.end()
        ? generic_payload.value().getString("updated_at")
        : util::trim(triggered_header->second);
    std::string triggered_at = raw_triggered_at;
    if (!raw_triggered_at.empty()) {
        auto normalized = shopify::ShopifyTimestamp::normalizeUtc(raw_triggered_at);
        if (!normalized) {
            const auto slash_position = topic.find('/');
            const auto invalid_resource_type = slash_position == std::string::npos
                ? topic : topic.substr(0, slash_position);
            std::string invalid_resource_id = generic_payload.value().getScalarString("admin_graphql_api_id");
            if (invalid_resource_id.empty()) {
                invalid_resource_id = generic_payload.value().getScalarString("id");
            }
            auto reconciliation = database_.requestShopifyReconciliation(
                request.headers.count("x-shopify-shop-domain")
                    ? util::trim(request.headers.at("x-shopify-shop-domain"))
                    : config_.shop + ".myshopify.com",
                invalid_resource_type, invalid_resource_id, "invalid_triggered_at",
                event_id, webhook_id, raw_triggered_at);
            if (!reconciliation) {
                database_.markWebhookProcessed("shopify", webhook_id, reconciliation.error());
                return {500, "application/json", R"({"error":"timestamp_reconciliation_failed"})"};
            }
            database_.markWebhookProcessed("shopify", webhook_id);
            if (counters_) ++counters_->accepted_events;
            return {202, "application/json",
                    R"({"accepted":true,"invalid_timestamp":true,"reconciliation_queued":true})"};
        }
        triggered_at = normalized.take();
    }
    const auto slash = topic.find('/');
    const std::string resource_type = slash == std::string::npos ? topic : topic.substr(0, slash);
    std::string resource_id = generic_payload.value().getScalarString("admin_graphql_api_id");
    if (resource_id.empty()) resource_id = generic_payload.value().getScalarString("id");

    if (!shopify::webhooks::isPrivacyTopic(parsed_topic) && !resource_id.empty()) {
        auto ordering = database_.evaluateShopifyWebhookOrder(
            request.headers.count("x-shopify-shop-domain")
                ? util::trim(request.headers.at("x-shopify-shop-domain"))
                : config_.shop + ".myshopify.com",
            resource_type, resource_id, triggered_at, event_id, webhook_id, payload_hash);
        if (!ordering) {
            database_.markWebhookProcessed("shopify", webhook_id, ordering.error());
            return {500, "application/json", R"({"error":"ordering_storage_failed"})"};
        }
        if (ordering.value().decision == ShopifyWebhookOrderDecision::duplicate) {
            database_.markWebhookProcessed("shopify", webhook_id);
            if (counters_) ++counters_->accepted_events;
            return {200, "application/json",
                    R"({"accepted":true,"duplicate_event":true})"};
        }
        if (ordering.value().decision == ShopifyWebhookOrderDecision::stale) {
            auto reconciliation = database_.requestShopifyReconciliation(
                request.headers.count("x-shopify-shop-domain")
                    ? util::trim(request.headers.at("x-shopify-shop-domain"))
                    : config_.shop + ".myshopify.com",
                resource_type, resource_id, ordering.value().reason,
                event_id, webhook_id, triggered_at);
            Json context = Json::object();
            context.set("topic", topic);
            context.set("resource_type", resource_type);
            context.set("resource_id", resource_id);
            context.set("triggered_at", triggered_at);
            context.set("previous_triggered_at", ordering.value().previous_triggered_at);
            context.set("reconciliation_queued", static_cast<bool>(reconciliation));
            database_.audit("WARNING", "webhook",
                            "Stale Shopify webhook suppressed and reconciliation requested",
                            context.dump(), webhook_id);
            if (!reconciliation) {
                database_.markWebhookProcessed("shopify", webhook_id, reconciliation.error());
                return {500, "application/json",
                        R"({"error":"reconciliation_queue_failure"})"};
            }
            database_.markWebhookProcessed("shopify", webhook_id);
            if (counters_) ++counters_->accepted_events;
            return {202, "application/json",
                    R"({"accepted":true,"stale":true,"reconciliation_queued":true})"};
        }
    }

    if (topic == "orders/paid" && order_manager_ != nullptr) {
        auto order = webhook_.parseOrderPaid(request.body);
        if (!order) {
            database_.markWebhookProcessed("shopify", webhook_id, order.error());
            if (counters_) ++counters_->rejected_events;
            return {400, "application/json", R"({"error":"invalid_order"})"};
        }
        auto ingested = order_manager_->ingest(order.take());
        database_.markWebhookProcessed("shopify", webhook_id,
                                       ingested ? std::string{} : ingested.error());
        if (!ingested) {
            if (counters_) ++counters_->rejected_events;
            return {400, "application/json", R"({"error":"order_rejected"})"};
        }
        if (counters_) ++counters_->accepted_events;
        return {202, "application/json", R"({"accepted":true,"processed":true})"};
    }

    Json payload = Json::object();
    payload.set("webhook_id", webhook_id);
    payload.set("topic", topic);
    payload.set("payload_hash", payload_hash);
    payload.set("event_id", event_id);
    payload.set("resource_type", resource_type);
    payload.set("resource_id", resource_id);
    payload.set("payload", request.body);
    if (version_header != request.headers.end())
        payload.set("api_version", util::trim(version_header->second));
    const auto triggered = request.headers.find("x-shopify-triggered-at");
    if (triggered != request.headers.end()) payload.set("triggered_at", util::trim(triggered->second));

    const auto task_kind = shopify::webhooks::taskKind(parsed_topic);
    auto queued = database_.enqueueUniqueTask(
        task_kind, payload.dump(), shopify::webhooks::isPrivacyTopic(parsed_topic) ? 1 : 10,
        {}, "shopify-webhook:" + webhook_id);
    if (!queued) {
        database_.markWebhookProcessed("shopify", webhook_id, queued.error());
        return {500, "application/json", R"({"error":"queue_failure"})"};
    }
    auto marked = database_.markWebhookQueued("shopify", webhook_id);
    if (!marked) return {500, "application/json", R"({"error":"webhook_status_failure"})"};
    Json audit = Json::object();
    audit.set("topic", topic);
    audit.set("task_kind", task_kind);
    audit.set("task_id", static_cast<std::int64_t>(queued.value().id));
    audit.set("known_topic", parsed_topic != shopify::webhooks::ShopifyWebhookTopic::unknown);
    database_.audit("INFO", "webhook", "Shopify webhook queued", audit.dump(), webhook_id);
    if (logger_) logger_->info("webhook", topic + " queued as " + task_kind);
    if (counters_) ++counters_->accepted_events;
    return {202, "application/json", R"({"accepted":true,"queued":true})"};
}

} // namespace elit21
