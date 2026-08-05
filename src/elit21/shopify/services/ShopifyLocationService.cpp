#include "elit21/shopify/services/ShopifyLocationService.h"

namespace elit21::shopify::services {

ShopifyLocationService::ShopifyLocationService()
    : platform::BusinessComponent("ShopifyLocationService","Typed Shopify Admin service",platform::BusinessComponentSpec{"shopify","orchestrate",{},true,true,4U*1024U*1024U}) {}

ShopifyLocationService::ShopifyLocationService(ShopifyClient& client) : ShopifyLocationService() { client_ = &client; }

ShopifyClient& ShopifyLocationService::requireClient() {
    if (!client_) throw std::logic_error("ShopifyLocationService is not bound to a ShopifyClient");
    return *client_;
}

Result<std::string> ShopifyLocationService::primaryLocation(){return requireClient().primaryLocationId();}

platform::OperationResult ShopifyLocationService::execute(const platform::OperationContext& context) {
    auto result = executeBusiness(context);
    result.metrics["client_bound"] = bound() ? 1.0 : 0.0;
    return result;
}

} // namespace elit21::shopify::services
