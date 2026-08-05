#include "elit21/mapping/SkuMapper.h"

namespace elit21::mapping {

SkuMapper::SkuMapper()
    : platform::BusinessComponent(
          "SkuMapper",
          "Shopify to AliExpress entity mapping",
          platform::BusinessComponentSpec{
              "mapping",
              "validate_transform",
              std::vector<std::string>{},
              false,
              false,
              4U * 1024U * 1024U}) {}

platform::OperationResult SkuMapper::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::mapping
