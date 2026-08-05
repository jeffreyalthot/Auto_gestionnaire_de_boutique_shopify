#include "elit21/mapping/ProductMapper.h"

namespace elit21::mapping {

ProductMapper::ProductMapper()
    : platform::BusinessComponent(
          "ProductMapper",
          "Shopify to AliExpress entity mapping",
          platform::BusinessComponentSpec{
              "mapping",
              "validate_transform",
              std::vector<std::string>{"product_id"},
              false,
              false,
              4U * 1024U * 1024U}) {}

platform::OperationResult ProductMapper::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::mapping
