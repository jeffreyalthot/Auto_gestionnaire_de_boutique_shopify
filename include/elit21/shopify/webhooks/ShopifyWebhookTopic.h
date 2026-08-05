#pragma once
#include <string>
#include <vector>
namespace elit21::shopify::webhooks {
enum class ShopifyWebhookTopic {orders_paid,orders_cancelled,refunds_create,products_update,products_delete,inventory_levels_update,fulfillments_create,fulfillments_update,app_uninstalled,customers_data_request,customers_redact,shop_redact,unknown};
[[nodiscard]] ShopifyWebhookTopic parseTopic(const std::string& value) noexcept;
[[nodiscard]] std::string topicName(ShopifyWebhookTopic topic);
[[nodiscard]] std::string taskKind(ShopifyWebhookTopic topic);
[[nodiscard]] bool isPrivacyTopic(ShopifyWebhookTopic topic) noexcept;
[[nodiscard]] bool isOrderTopic(ShopifyWebhookTopic topic) noexcept;
[[nodiscard]] std::vector<ShopifyWebhookTopic> requiredTopics();
}
