#include "elit21/mapping/ProductMapping.h"

namespace elit21::mapping {

ProductMapping::ProductMapping()
    : platform::BusinessComponent(
          "ProductMapping",
          "Shopify to AliExpress entity mapping",
          platform::BusinessComponentSpec{
              "mapping",
              "model_operation",
              std::vector<std::string>{},
              false,
              false,
              4U * 1024U * 1024U}) {}

platform::OperationResult ProductMapping::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::mapping
