#include "elit21/mapping/CategoryMapper.h"

namespace elit21::mapping {

CategoryMapper::CategoryMapper()
    : platform::BusinessComponent(
          "CategoryMapper",
          "Shopify to AliExpress entity mapping",
          platform::BusinessComponentSpec{
              "mapping",
              "validate_transform",
              std::vector<std::string>{},
              false,
              false,
              4U * 1024U * 1024U}) {}

platform::OperationResult CategoryMapper::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::mapping
