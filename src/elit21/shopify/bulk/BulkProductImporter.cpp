#include "elit21/shopify/bulk/BulkProductImporter.h"
namespace elit21::shopify::bulk {
BulkProductImporter::BulkProductImporter():platform::BusinessComponent("BulkProductImporter","Validated Shopify bulk product importer",platform::BusinessComponentSpec{"shopify","import_bulk_products",{"path"},true,false,4U*1024U*1024U}){}
Result<BulkJsonlStatistics>BulkProductImporter::importFile(const std::string&path,const ProductConsumer&consumer,std::size_t maximum)const{BulkJsonlParser parser;std::size_t products=0;return parser.parseFile(path,[&](const BulkJsonlRecord&r){if(r.parent_id.empty()&&r.value.contains("title")){if(++products>maximum)return Result<void>::failure("Bulk product limit reached");if(consumer)return consumer(r.value);}return Result<void>::success();});}
platform::OperationResult BulkProductImporter::execute(const platform::OperationContext&context){return executeBusiness(context);}}
