#include "elit21/shopify/webhooks/ShopifyWebhookTopic.h"
#include "elit21/util/StringUtil.h"

#include <unordered_map>

namespace elit21::shopify::webhooks {

ShopifyWebhookTopic parseTopic(const std::string& value) noexcept {
    static const std::unordered_map<std::string, ShopifyWebhookTopic> mapping{
        {"orders/paid", ShopifyWebhookTopic::orders_paid},
        {"orders/cancelled", ShopifyWebhookTopic::orders_cancelled},
        {"refunds/create", ShopifyWebhookTopic::refunds_create},
        {"products/update", ShopifyWebhookTopic::products_update},
        {"products/delete", ShopifyWebhookTopic::products_delete},
        {"inventory_levels/update", ShopifyWebhookTopic::inventory_levels_update},
        {"fulfillments/create", ShopifyWebhookTopic::fulfillments_create},
        {"fulfillments/update", ShopifyWebhookTopic::fulfillments_update},
        {"app/uninstalled", ShopifyWebhookTopic::app_uninstalled},
        {"customers/data_request", ShopifyWebhookTopic::customers_data_request},
        {"customers/redact", ShopifyWebhookTopic::customers_redact},
        {"shop/redact", ShopifyWebhookTopic::shop_redact}
    };
    const auto found = mapping.find(util::lower(util::trim(value)));
    return found == mapping.end() ? ShopifyWebhookTopic::unknown : found->second;
}

std::string topicName(ShopifyWebhookTopic topic) {
    switch (topic) {
        case ShopifyWebhookTopic::orders_paid: return "orders/paid";
        case ShopifyWebhookTopic::orders_cancelled: return "orders/cancelled";
        case ShopifyWebhookTopic::refunds_create: return "refunds/create";
        case ShopifyWebhookTopic::products_update: return "products/update";
        case ShopifyWebhookTopic::products_delete: return "products/delete";
        case ShopifyWebhookTopic::inventory_levels_update: return "inventory_levels/update";
        case ShopifyWebhookTopic::fulfillments_create: return "fulfillments/create";
        case ShopifyWebhookTopic::fulfillments_update: return "fulfillments/update";
        case ShopifyWebhookTopic::app_uninstalled: return "app/uninstalled";
        case ShopifyWebhookTopic::customers_data_request: return "customers/data_request";
        case ShopifyWebhookTopic::customers_redact: return "customers/redact";
        case ShopifyWebhookTopic::shop_redact: return "shop/redact";
        default: return "unknown";
    }
}

std::string taskKind(ShopifyWebhookTopic topic) {
    switch (topic) {
        case ShopifyWebhookTopic::orders_paid: return "shopify_order_paid";
        case ShopifyWebhookTopic::orders_cancelled: return "shopify_order_cancelled";
        case ShopifyWebhookTopic::refunds_create: return "shopify_refund_created";
        case ShopifyWebhookTopic::products_update: return "shopify_product_updated";
        case ShopifyWebhookTopic::products_delete: return "shopify_product_deleted";
        case ShopifyWebhookTopic::inventory_levels_update: return "shopify_inventory_updated";
        case ShopifyWebhookTopic::fulfillments_create:
        case ShopifyWebhookTopic::fulfillments_update: return "shopify_fulfillment_updated";
        case ShopifyWebhookTopic::app_uninstalled: return "shopify_app_uninstalled";
        case ShopifyWebhookTopic::customers_data_request: return "shopify_customers_data_request";
        case ShopifyWebhookTopic::customers_redact: return "shopify_customers_redact";
        case ShopifyWebhookTopic::shop_redact: return "shopify_shop_redact";
        default: return "shopify_webhook_unknown";
    }
}

bool isPrivacyTopic(ShopifyWebhookTopic topic) noexcept {
    return topic == ShopifyWebhookTopic::customers_data_request ||
           topic == ShopifyWebhookTopic::customers_redact || topic == ShopifyWebhookTopic::shop_redact;
}

bool isOrderTopic(ShopifyWebhookTopic topic) noexcept {
    return topic == ShopifyWebhookTopic::orders_paid || topic == ShopifyWebhookTopic::orders_cancelled;
}

std::vector<ShopifyWebhookTopic> requiredTopics() {
    return {ShopifyWebhookTopic::orders_paid, ShopifyWebhookTopic::orders_cancelled,
            ShopifyWebhookTopic::refunds_create, ShopifyWebhookTopic::products_update,
            ShopifyWebhookTopic::products_delete, ShopifyWebhookTopic::inventory_levels_update,
            ShopifyWebhookTopic::fulfillments_create, ShopifyWebhookTopic::fulfillments_update,
            ShopifyWebhookTopic::app_uninstalled, ShopifyWebhookTopic::customers_data_request,
            ShopifyWebhookTopic::customers_redact, ShopifyWebhookTopic::shop_redact};
}

} // namespace elit21::shopify::webhooks
