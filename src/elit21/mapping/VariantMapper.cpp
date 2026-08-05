#include "elit21/mapping/VariantMapper.h"

namespace elit21::mapping {

VariantMapper::VariantMapper()
    : platform::BusinessComponent(
          "VariantMapper",
          "Shopify to AliExpress entity mapping",
          platform::BusinessComponentSpec{
              "mapping",
              "validate_transform",
              std::vector<std::string>{},
              false,
              false,
              4U * 1024U * 1024U}) {}

platform::OperationResult VariantMapper::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::mapping
