#include "elit21/mapping/CustomerMapper.h"

namespace elit21::mapping {

CustomerMapper::CustomerMapper()
    : platform::BusinessComponent(
          "CustomerMapper",
          "Shopify to AliExpress entity mapping",
          platform::BusinessComponentSpec{
              "mapping",
              "validate_transform",
              std::vector<std::string>{"customer_id"},
              false,
              false,
              4U * 1024U * 1024U}) {}

platform::OperationResult CustomerMapper::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::mapping
