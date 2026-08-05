#include "elit21/shopify/services/ShopifyOrderService.h"

namespace elit21::shopify::services {

ShopifyOrderService::ShopifyOrderService()
    : platform::BusinessComponent("ShopifyOrderService","Typed Shopify Admin service",platform::BusinessComponentSpec{"shopify","orchestrate",{},true,true,4U*1024U*1024U}) {}

ShopifyOrderService::ShopifyOrderService(ShopifyClient& client) : ShopifyOrderService() { client_ = &client; }

ShopifyClient& ShopifyOrderService::requireClient() {
    if (!client_) throw std::logic_error("ShopifyOrderService is not bound to a ShopifyClient");
    return *client_;
}

Result<ShopifyOrderPage> ShopifyOrderService::paidUnfulfilledPage(int first,const std::string& after){return requireClient().fetchOpenPaidOrdersPage(first,after);}
Result<std::vector<CustomerOrder>> ShopifyOrderService::paidUnfulfilled(int maximum_orders,int max_pages){return requireClient().fetchOpenPaidOrders(maximum_orders,max_pages);}

platform::OperationResult ShopifyOrderService::execute(const platform::OperationContext& context) {
    auto result = executeBusiness(context);
    result.metrics["client_bound"] = bound() ? 1.0 : 0.0;
    return result;
}

} // namespace elit21::shopify::services
