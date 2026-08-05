#include "elit21/shopify/webhooks/ShopifyWebhook.h"

#include "elit21/security/Crypto.h"
#include "elit21/shopify/ShopifyEndpoints.h"

namespace elit21::shopify::webhooks {

ShopifyWebhook::ShopifyWebhook(std::string id,
                               ShopifyWebhookTopic topic,
                               std::string shop_domain,
                               std::string api_version,
                               std::string body,
                               Json payload,
                               std::string event_id,
                               std::string triggered_at,
                               std::string subscription_name)
    : id_(std::move(id)),
      topic_(topic),
      shop_domain_(std::move(shop_domain)),
      api_version_(std::move(api_version)),
      body_(std::move(body)),
      payload_(std::move(payload)),
      event_id_(std::move(event_id)),
      triggered_at_(std::move(triggered_at)),
      subscription_name_(std::move(subscription_name)) {}

std::string ShopifyWebhook::payloadHash() const {
    return crypto::sha256Hex(body_);
}


std::string ShopifyWebhook::resourceType() const {
    const auto topic = topicName(topic_);
    const auto slash = topic.find('/');
    return slash == std::string::npos ? topic : topic.substr(0, slash);
}

std::string ShopifyWebhook::resourceId() const {
    auto value = payload_.getScalarString("admin_graphql_api_id");
    if (!value.empty()) return value;
    value = payload_.getScalarString("id");
    if (!value.empty()) return value;
    const auto order = payload_.get("order");
    value = order.getScalarString("admin_graphql_api_id");
    if (!value.empty()) return value;
    return order.getScalarString("id");
}

std::string ShopifyWebhook::sourceUpdatedAt() const {
    auto value = payload_.getString("updated_at");
    if (!value.empty()) return value;
    value = payload_.getString("processed_at");
    if (!value.empty()) return value;
    return triggered_at_;
}

std::string ShopifyWebhook::dedupeKey() const {
    return "shopify-webhook:" + (id_.empty() ? payloadHash() : id_);
}

Result<void> ShopifyWebhook::validate() const {
    if (id_.empty()) return Result<void>::failure("Shopify webhook id is empty");
    if (topic_ == ShopifyWebhookTopic::unknown) {
        return Result<void>::failure("Shopify webhook topic is unknown");
    }
    if (!ShopifyEndpoints::isTrustedShopDomain(shop_domain_)) {
        return Result<void>::failure("Shopify webhook shop domain is not trusted");
    }
    if (body_.empty() || !payload_.isObject()) {
        return Result<void>::failure("Shopify webhook payload is empty or invalid");
    }
    return Result<void>::success();
}

Json ShopifyWebhook::toJson(bool include_payload) const {
    Json output = Json::object();
    output.set("id", id_);
    output.set("topic", topicName(topic_));
    output.set("shop_domain", shop_domain_);
    output.set("api_version", api_version_);
    output.set("payload_hash", payloadHash());
    output.set("event_id", event_id_);
    output.set("triggered_at", triggered_at_);
    output.set("subscription_name", subscription_name_);
    output.set("resource_type", resourceType());
    output.set("resource_id", resourceId());
    output.set("source_updated_at", sourceUpdatedAt());
    output.set("privacy_topic", isPrivacyTopic(topic_));
    if (include_payload) output.set("payload", payload_);
    return output;
}

} // namespace elit21::shopify::webhooks
