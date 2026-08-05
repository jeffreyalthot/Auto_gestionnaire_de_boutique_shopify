#pragma once

#include "elit21/core/Result.h"
#include "elit21/json/Json.h"
#include "elit21/shopify/webhooks/ShopifyWebhookTopic.h"

#include <string>

namespace elit21::shopify::webhooks {

class ShopifyWebhook {
public:
    ShopifyWebhook() = default;
    ShopifyWebhook(std::string id,
                   ShopifyWebhookTopic topic,
                   std::string shop_domain,
                   std::string api_version,
                   std::string body,
                   Json payload,
                   std::string event_id = {},
                   std::string triggered_at = {},
                   std::string subscription_name = {});

    [[nodiscard]] const std::string& id() const noexcept { return id_; }
    [[nodiscard]] ShopifyWebhookTopic topic() const noexcept { return topic_; }
    [[nodiscard]] const std::string& shopDomain() const noexcept { return shop_domain_; }
    [[nodiscard]] const std::string& apiVersion() const noexcept { return api_version_; }
    [[nodiscard]] const std::string& body() const noexcept { return body_; }
    [[nodiscard]] const Json& payload() const noexcept { return payload_; }
    [[nodiscard]] const std::string& eventId() const noexcept { return event_id_; }
    [[nodiscard]] const std::string& triggeredAt() const noexcept { return triggered_at_; }
    [[nodiscard]] const std::string& subscriptionName() const noexcept { return subscription_name_; }
    [[nodiscard]] std::string payloadHash() const;
    [[nodiscard]] std::string resourceType() const;
    [[nodiscard]] std::string resourceId() const;
    [[nodiscard]] std::string sourceUpdatedAt() const;
    [[nodiscard]] std::string dedupeKey() const;
    [[nodiscard]] Result<void> validate() const;
    [[nodiscard]] Json toJson(bool include_payload = false) const;

private:
    std::string id_;
    ShopifyWebhookTopic topic_{ShopifyWebhookTopic::unknown};
    std::string shop_domain_;
    std::string api_version_;
    std::string body_;
    Json payload_{Json::object()};
    std::string event_id_;
    std::string triggered_at_;
    std::string subscription_name_;
};

} // namespace elit21::shopify::webhooks
