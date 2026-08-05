#include "elit21/shopify/bulk/BulkInventorySynchronizer.h"
#include "elit21/util/StringUtil.h"
namespace elit21::shopify::bulk {
BulkInventorySynchronizer::BulkInventorySynchronizer():platform::BusinessComponent("BulkInventorySynchronizer","Shopify bulk synchronize",platform::BusinessComponentSpec{"shopify","bulk_synchronize",{},true,true,4U*1024U*1024U}){}
BulkInventorySynchronizer::BulkInventorySynchronizer(ShopifyClient& client):BulkInventorySynchronizer(){client_=&client;}
Result<ShopifyBulkOperationRef> BulkInventorySynchronizer::start(const std::string& search_filter){
    if(!client_)return Result<ShopifyBulkOperationRef>::failure("BulkInventorySynchronizer is not bound");
    std::string query=R"graphql({ inventoryItems { edges { node { id sku tracked variant{id product{id}} inventoryLevels(first:100){edges{node{id quantities(names:["available"]){name quantity} location{id name}}}} } } } })graphql";
    if(!search_filter.empty()) query="{ # ELIT21 filter: "+util::trim(search_filter)+"\n"+query.substr(1);
    return client_->runBulkQuery(query);
}
platform::OperationResult BulkInventorySynchronizer::execute(const platform::OperationContext& context){auto r=executeBusiness(context);r.metrics["client_bound"]=client_?1.0:0.0;r.attributes["bulk_mode"]="asynchronous";return r;}
}
