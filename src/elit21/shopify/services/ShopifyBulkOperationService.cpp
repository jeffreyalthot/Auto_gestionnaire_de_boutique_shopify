#include "elit21/shopify/services/ShopifyBulkOperationService.h"

namespace elit21::shopify::services {

ShopifyBulkOperationService::ShopifyBulkOperationService()
    : platform::BusinessComponent("ShopifyBulkOperationService","Typed Shopify Admin service",platform::BusinessComponentSpec{"shopify","orchestrate",{},true,true,4U*1024U*1024U}) {}

ShopifyBulkOperationService::ShopifyBulkOperationService(ShopifyClient& client) : ShopifyBulkOperationService() { client_ = &client; }

ShopifyClient& ShopifyBulkOperationService::requireClient() {
    if (!client_) throw std::logic_error("ShopifyBulkOperationService is not bound to a ShopifyClient");
    return *client_;
}

Result<ShopifyBulkOperationRef> ShopifyBulkOperationService::runQuery(const std::string& query){return requireClient().runBulkQuery(query);}
Result<std::vector<ShopifyBulkOperationRef>> ShopifyBulkOperationService::list(int first,const std::string& filter){return requireClient().bulkOperations(first,filter);}
Result<ShopifyBulkOperationRef> ShopifyBulkOperationService::get(const std::string& operation_gid){return requireClient().bulkOperation(operation_gid);}
Result<void> ShopifyBulkOperationService::cancel(const std::string& operation_gid){return requireClient().cancelBulkOperation(operation_gid);}

platform::OperationResult ShopifyBulkOperationService::execute(const platform::OperationContext& context) {
    auto result = executeBusiness(context);
    result.metrics["client_bound"] = bound() ? 1.0 : 0.0;
    return result;
}

} // namespace elit21::shopify::services
