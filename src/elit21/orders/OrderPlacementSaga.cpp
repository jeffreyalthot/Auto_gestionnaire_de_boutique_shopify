#include "elit21/orders/OrderPlacementSaga.h"

namespace elit21::orders {

OrderPlacementSaga::OrderPlacementSaga()
    : platform::BusinessComponent(
          "OrderPlacementSaga",
          "order validation and supplier placement workflow",
          platform::BusinessComponentSpec{
              "orders",
              "command",
              std::vector<std::string>{"order_id"},
              true,
              false,
              4U * 1024U * 1024U}) {}

platform::OperationResult OrderPlacementSaga::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::orders
