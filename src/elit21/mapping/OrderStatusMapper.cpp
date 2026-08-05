#include "elit21/mapping/OrderStatusMapper.h"

namespace elit21::mapping {

OrderStatusMapper::OrderStatusMapper()
    : platform::BusinessComponent(
          "OrderStatusMapper",
          "Shopify to AliExpress entity mapping",
          platform::BusinessComponentSpec{
              "mapping",
              "validate_transform",
              std::vector<std::string>{"order_id"},
              false,
              false,
              4U * 1024U * 1024U}) {}

platform::OperationResult OrderStatusMapper::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::mapping
