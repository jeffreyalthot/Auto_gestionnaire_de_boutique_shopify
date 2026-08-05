#include "elit21/mapping/ProductMappingService.h"

namespace elit21::mapping {

ProductMappingService::ProductMappingService()
    : platform::BusinessComponent(
          "ProductMappingService",
          "Shopify to AliExpress entity mapping",
          platform::BusinessComponentSpec{
              "mapping",
              "orchestrate",
              std::vector<std::string>{},
              true,
              false,
              4U * 1024U * 1024U}) {}

platform::OperationResult ProductMappingService::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::mapping
