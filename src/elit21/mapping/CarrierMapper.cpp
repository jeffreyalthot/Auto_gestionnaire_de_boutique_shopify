#include "elit21/mapping/CarrierMapper.h"

namespace elit21::mapping {

CarrierMapper::CarrierMapper()
    : platform::BusinessComponent(
          "CarrierMapper",
          "Shopify to AliExpress entity mapping",
          platform::BusinessComponentSpec{
              "mapping",
              "validate_transform",
              std::vector<std::string>{},
              false,
              false,
              4U * 1024U * 1024U}) {}

platform::OperationResult CarrierMapper::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::mapping
