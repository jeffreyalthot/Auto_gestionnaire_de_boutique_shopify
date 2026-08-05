#include "elit21/shopify/services/ShopifyFulfillmentOrderService.h"

namespace elit21::shopify::services {

ShopifyFulfillmentOrderService::ShopifyFulfillmentOrderService()
    : platform::BusinessComponent("ShopifyFulfillmentOrderService","Typed Shopify Admin service",platform::BusinessComponentSpec{"shopify","orchestrate",{},true,true,4U*1024U*1024U}) {}

ShopifyFulfillmentOrderService::ShopifyFulfillmentOrderService(ShopifyClient& client) : ShopifyFulfillmentOrderService() { client_ = &client; }

ShopifyClient& ShopifyFulfillmentOrderService::requireClient() {
    if (!client_) throw std::logic_error("ShopifyFulfillmentOrderService is not bound to a ShopifyClient");
    return *client_;
}

Result<std::vector<ShopifyFulfillmentOrderRef>> ShopifyFulfillmentOrderService::forOrder(const std::string& order_gid){return requireClient().fulfillmentOrders(order_gid);}

platform::OperationResult ShopifyFulfillmentOrderService::execute(const platform::OperationContext& context) {
    auto result = executeBusiness(context);
    result.metrics["client_bound"] = bound() ? 1.0 : 0.0;
    return result;
}

} // namespace elit21::shopify::services
