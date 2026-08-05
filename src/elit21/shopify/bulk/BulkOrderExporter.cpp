#include "elit21/shopify/bulk/BulkOrderExporter.h"
#include "elit21/util/StringUtil.h"
namespace elit21::shopify::bulk {
BulkOrderExporter::BulkOrderExporter():platform::BusinessComponent("BulkOrderExporter","Shopify bulk export",platform::BusinessComponentSpec{"shopify","bulk_export",{},true,true,4U*1024U*1024U}){}
BulkOrderExporter::BulkOrderExporter(ShopifyClient& client):BulkOrderExporter(){client_=&client;}
Result<ShopifyBulkOperationRef> BulkOrderExporter::start(const std::string& search_filter){
    if(!client_)return Result<ShopifyBulkOperationRef>::failure("BulkOrderExporter is not bound");
    std::string query=R"graphql({ orders(query:"financial_status:paid",sortKey:UPDATED_AT){edges{node{id name createdAt updatedAt displayFinancialStatus displayFulfillmentStatus totalPriceSet{shopMoney{amount currencyCode}} lineItems(first:100){edges{node{id sku quantity originalUnitPriceSet{shopMoney{amount currencyCode}}}}}}}}})graphql";
    if(!search_filter.empty()) query="{ # ELIT21 filter: "+util::trim(search_filter)+"\n"+query.substr(1);
    return client_->runBulkQuery(query);
}
platform::OperationResult BulkOrderExporter::execute(const platform::OperationContext& context){auto r=executeBusiness(context);r.metrics["client_bound"]=client_?1.0:0.0;r.attributes["bulk_mode"]="asynchronous";return r;}
}
