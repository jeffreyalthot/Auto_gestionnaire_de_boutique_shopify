#include "elit21/shopify/services/ShopifyShopService.h"

namespace elit21::shopify::services {

ShopifyShopService::ShopifyShopService()
    : platform::BusinessComponent("ShopifyShopService","Typed Shopify Admin service",platform::BusinessComponentSpec{"shopify","orchestrate",{},true,true,4U*1024U*1024U}) {}

ShopifyShopService::ShopifyShopService(ShopifyClient& client) : ShopifyShopService() { client_ = &client; }

ShopifyClient& ShopifyShopService::requireClient() {
    if (!client_) throw std::logic_error("ShopifyShopService is not bound to a ShopifyClient");
    return *client_;
}

Result<void> ShopifyShopService::healthCheck(){return requireClient().healthCheck();}

platform::OperationResult ShopifyShopService::execute(const platform::OperationContext& context) {
    auto result = executeBusiness(context);
    result.metrics["client_bound"] = bound() ? 1.0 : 0.0;
    return result;
}

} // namespace elit21::shopify::services
