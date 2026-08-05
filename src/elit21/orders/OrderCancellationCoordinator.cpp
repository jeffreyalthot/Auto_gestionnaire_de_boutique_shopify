#include "elit21/orders/OrderCancellationCoordinator.h"

namespace elit21::orders {

OrderCancellationCoordinator::OrderCancellationCoordinator()
    : platform::BusinessComponent(
          "OrderCancellationCoordinator",
          "order validation and supplier placement workflow",
          platform::BusinessComponentSpec{
              "orders",
              "command",
              std::vector<std::string>{"order_id"},
              true,
              false,
              4U * 1024U * 1024U}) {}

platform::OperationResult OrderCancellationCoordinator::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::orders
