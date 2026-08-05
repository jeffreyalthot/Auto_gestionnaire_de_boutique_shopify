#include "elit21/shopify/services/ShopifyVariantService.h"

namespace elit21::shopify::services {

ShopifyVariantService::ShopifyVariantService()
    : platform::BusinessComponent("ShopifyVariantService","Typed Shopify Admin service",platform::BusinessComponentSpec{"shopify","orchestrate",{},true,true,4U*1024U*1024U}) {}

ShopifyVariantService::ShopifyVariantService(ShopifyClient& client) : ShopifyVariantService() { client_ = &client; }

ShopifyClient& ShopifyVariantService::requireClient() {
    if (!client_) throw std::logic_error("ShopifyVariantService is not bound to a ShopifyClient");
    return *client_;
}

Result<std::vector<ShopifyVariantRef>> ShopifyVariantService::create(const std::string& product_gid,const std::vector<ShopifyProductVariantCreate>& variants,bool remove_standalone){return requireClient().createVariants(product_gid,variants,remove_standalone);}
Result<void> ShopifyVariantService::updatePrices(const std::string& product_gid,const std::vector<ShopifyVariantPriceUpdate>& updates){return requireClient().updateVariantPrices(product_gid,updates);}

platform::OperationResult ShopifyVariantService::execute(const platform::OperationContext& context) {
    auto result = executeBusiness(context);
    result.metrics["client_bound"] = bound() ? 1.0 : 0.0;
    return result;
}

} // namespace elit21::shopify::services
