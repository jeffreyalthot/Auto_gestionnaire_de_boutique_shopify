#include "elit21/orders/OrderFraudScreen.h"

namespace elit21::orders {

OrderFraudScreen::OrderFraudScreen()
    : platform::BusinessComponent(
          "OrderFraudScreen",
          "order validation and supplier placement workflow",
          platform::BusinessComponentSpec{
              "orders",
              "model_operation",
              std::vector<std::string>{},
              false,
              false,
              4U * 1024U * 1024U}) {}

platform::OperationResult OrderFraudScreen::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::orders
