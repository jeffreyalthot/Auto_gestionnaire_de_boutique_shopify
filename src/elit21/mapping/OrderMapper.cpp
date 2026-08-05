#include "elit21/mapping/OrderMapper.h"

namespace elit21::mapping {

OrderMapper::OrderMapper()
    : platform::BusinessComponent(
          "OrderMapper",
          "Shopify to AliExpress entity mapping",
          platform::BusinessComponentSpec{
              "mapping",
              "validate_transform",
              std::vector<std::string>{"order_id"},
              false,
              false,
              4U * 1024U * 1024U}) {}

platform::OperationResult OrderMapper::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::mapping
