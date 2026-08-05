#include "elit21/orders/OrderExceptionHandler.h"

namespace elit21::orders {

OrderExceptionHandler::OrderExceptionHandler()
    : platform::BusinessComponent(
          "OrderExceptionHandler",
          "order validation and supplier placement workflow",
          platform::BusinessComponentSpec{
              "orders",
              "model_operation",
              std::vector<std::string>{},
              false,
              false,
              4U * 1024U * 1024U}) {}

platform::OperationResult OrderExceptionHandler::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::orders
