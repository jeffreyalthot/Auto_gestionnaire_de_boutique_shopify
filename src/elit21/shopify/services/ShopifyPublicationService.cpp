#include "elit21/shopify/services/ShopifyPublicationService.h"

namespace elit21::shopify::services {

ShopifyPublicationService::ShopifyPublicationService()
    : platform::BusinessComponent("ShopifyPublicationService","Typed Shopify Admin service",platform::BusinessComponentSpec{"shopify","orchestrate",{},true,true,4U*1024U*1024U}) {}

ShopifyPublicationService::ShopifyPublicationService(ShopifyClient& client) : ShopifyPublicationService() { client_ = &client; }

ShopifyClient& ShopifyPublicationService::requireClient() {
    if (!client_) throw std::logic_error("ShopifyPublicationService is not bound to a ShopifyClient");
    return *client_;
}

Result<std::string> ShopifyPublicationService::primaryPublication(){return requireClient().primaryPublicationId();}
Result<void> ShopifyPublicationService::publish(const std::string& product_gid,const std::string& publication_gid){return requireClient().publishProduct(product_gid,publication_gid);}

platform::OperationResult ShopifyPublicationService::execute(const platform::OperationContext& context) {
    auto result = executeBusiness(context);
    result.metrics["client_bound"] = bound() ? 1.0 : 0.0;
    return result;
}

} // namespace elit21::shopify::services
