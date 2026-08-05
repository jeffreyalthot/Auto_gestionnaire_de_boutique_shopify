#include "elit21/mapping/AddressMapper.h"

namespace elit21::mapping {

AddressMapper::AddressMapper()
    : platform::BusinessComponent(
          "AddressMapper",
          "Shopify to AliExpress entity mapping",
          platform::BusinessComponentSpec{
              "mapping",
              "validate_transform",
              std::vector<std::string>{},
              false,
              false,
              4U * 1024U * 1024U}) {}

platform::OperationResult AddressMapper::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::mapping
