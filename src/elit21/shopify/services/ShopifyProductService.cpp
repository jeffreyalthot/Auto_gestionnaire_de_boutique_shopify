#include "elit21/shopify/services/ShopifyProductService.h"

namespace elit21::shopify::services {

ShopifyProductService::ShopifyProductService()
    : platform::BusinessComponent("ShopifyProductService","Typed Shopify Admin service",platform::BusinessComponentSpec{"shopify","orchestrate",{},true,true,4U*1024U*1024U}) {}

ShopifyProductService::ShopifyProductService(ShopifyClient& client) : ShopifyProductService() { client_ = &client; }

ShopifyClient& ShopifyProductService::requireClient() {
    if (!client_) throw std::logic_error("ShopifyProductService is not bound to a ShopifyClient");
    return *client_;
}

Result<std::string> ShopifyProductService::create(const ShopifyProductCreate& product){return requireClient().createProduct(product);}
Result<std::vector<ShopifyVariantRef>> ShopifyProductService::variants(const std::string& product_gid){return requireClient().productVariants(product_gid);}

platform::OperationResult ShopifyProductService::execute(const platform::OperationContext& context) {
    auto result = executeBusiness(context);
    result.metrics["client_bound"] = bound() ? 1.0 : 0.0;
    return result;
}

} // namespace elit21::shopify::services
