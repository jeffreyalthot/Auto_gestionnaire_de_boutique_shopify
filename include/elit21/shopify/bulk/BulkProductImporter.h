#pragma once
#include "elit21/core/Result.h"
#include "elit21/platform/BusinessComponent.h"
#include "elit21/shopify/bulk/BulkJsonlParser.h"
#include <cstddef>
#include <functional>
#include <string>
namespace elit21::shopify::bulk {
class BulkProductImporter final:public platform::BusinessComponent{
public:using ProductConsumer=std::function<Result<void>(const Json&)>;BulkProductImporter();Result<BulkJsonlStatistics>importFile(const std::string&path,const ProductConsumer&consumer,std::size_t maximum_products=100000)const;platform::OperationResult execute(const platform::OperationContext&context)override;};}
