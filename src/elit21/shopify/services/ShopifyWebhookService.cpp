#include "elit21/shopify/services/ShopifyWebhookService.h"

namespace elit21::shopify::services {

ShopifyWebhookService::ShopifyWebhookService()
    : platform::BusinessComponent("ShopifyWebhookService","Typed Shopify Admin service",platform::BusinessComponentSpec{"shopify","orchestrate",{},true,true,4U*1024U*1024U}) {}

ShopifyWebhookService::ShopifyWebhookService(ShopifyClient& client) : ShopifyWebhookService() { client_ = &client; }

ShopifyClient& ShopifyWebhookService::requireClient() {
    if (!client_) throw std::logic_error("ShopifyWebhookService is not bound to a ShopifyClient");
    return *client_;
}

Result<void> ShopifyWebhookService::registerTopic(const std::string& topic,const std::string& callback_url){return requireClient().registerWebhook(topic,callback_url);}
Result<std::vector<ShopifyWebhookSubscriptionRef>> ShopifyWebhookService::subscriptions(int first){return requireClient().webhookSubscriptions(first);}
Result<void> ShopifyWebhookService::remove(const std::string& subscription_gid){return requireClient().deleteWebhook(subscription_gid);}

platform::OperationResult ShopifyWebhookService::execute(const platform::OperationContext& context) {
    auto result = executeBusiness(context);
    result.metrics["client_bound"] = bound() ? 1.0 : 0.0;
    return result;
}

} // namespace elit21::shopify::services
