#include "elit21/mapping/ShippingMethodMapper.h"

namespace elit21::mapping {

ShippingMethodMapper::ShippingMethodMapper()
    : platform::BusinessComponent(
          "ShippingMethodMapper",
          "Shopify to AliExpress entity mapping",
          platform::BusinessComponentSpec{
              "mapping",
              "validate_transform",
              std::vector<std::string>{},
              false,
              false,
              4U * 1024U * 1024U}) {}

platform::OperationResult ShippingMethodMapper::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::mapping
