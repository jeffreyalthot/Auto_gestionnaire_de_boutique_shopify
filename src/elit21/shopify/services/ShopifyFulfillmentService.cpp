#include "elit21/shopify/services/ShopifyFulfillmentService.h"

namespace elit21::shopify::services {

ShopifyFulfillmentService::ShopifyFulfillmentService()
    : platform::BusinessComponent("ShopifyFulfillmentService","Typed Shopify Admin service",platform::BusinessComponentSpec{"shopify","orchestrate",{},true,true,4U*1024U*1024U}) {}

ShopifyFulfillmentService::ShopifyFulfillmentService(ShopifyClient& client) : ShopifyFulfillmentService() { client_ = &client; }

ShopifyClient& ShopifyFulfillmentService::requireClient() {
    if (!client_) throw std::logic_error("ShopifyFulfillmentService is not bound to a ShopifyClient");
    return *client_;
}

Result<void> ShopifyFulfillmentService::create(const std::string& fulfillment_order_gid,const ShipmentInfo& tracking,bool notify_customer){return requireClient().createFulfillment(fulfillment_order_gid,tracking,notify_customer);}

platform::OperationResult ShopifyFulfillmentService::execute(const platform::OperationContext& context) {
    auto result = executeBusiness(context);
    result.metrics["client_bound"] = bound() ? 1.0 : 0.0;
    return result;
}

} // namespace elit21::shopify::services
