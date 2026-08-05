#include "elit21/orders/CustomerOrderLine.h"

namespace elit21::orders {

CustomerOrderLine::CustomerOrderLine()
    : platform::BusinessComponent(
          "CustomerOrderLine",
          "order validation and supplier placement workflow",
          platform::BusinessComponentSpec{
              "orders",
              "model_operation",
              std::vector<std::string>{},
              false,
              false,
              4U * 1024U * 1024U}) {}

platform::OperationResult CustomerOrderLine::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::orders
