#include "elit21/shopify/bulk/BulkProductExporter.h"
#include "elit21/util/StringUtil.h"
namespace elit21::shopify::bulk {
BulkProductExporter::BulkProductExporter():platform::BusinessComponent("BulkProductExporter","Shopify bulk export",platform::BusinessComponentSpec{"shopify","bulk_export",{},true,true,4U*1024U*1024U}){}
BulkProductExporter::BulkProductExporter(ShopifyClient& client):BulkProductExporter(){client_=&client;}
Result<ShopifyBulkOperationRef> BulkProductExporter::start(const std::string& search_filter){
    if(!client_)return Result<ShopifyBulkOperationRef>::failure("BulkProductExporter is not bound");
    std::string query=R"graphql({ products { edges { node { id title handle status updatedAt variants(first:100){edges{node{id sku title price inventoryQuantity inventoryItem{id}}}} } } } })graphql";
    if(!search_filter.empty()) query="{ # ELIT21 filter: "+util::trim(search_filter)+"\n"+query.substr(1);
    return client_->runBulkQuery(query);
}
platform::OperationResult BulkProductExporter::execute(const platform::OperationContext& context){auto r=executeBusiness(context);r.metrics["client_bound"]=client_?1.0:0.0;r.attributes["bulk_mode"]="asynchronous";return r;}
}
