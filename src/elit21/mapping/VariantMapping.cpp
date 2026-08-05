#include "elit21/mapping/VariantMapping.h"

namespace elit21::mapping {

VariantMapping::VariantMapping()
    : platform::BusinessComponent(
          "VariantMapping",
          "Shopify to AliExpress entity mapping",
          platform::BusinessComponentSpec{
              "mapping",
              "model_operation",
              std::vector<std::string>{},
              false,
              false,
              4U * 1024U * 1024U}) {}

platform::OperationResult VariantMapping::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::mapping
