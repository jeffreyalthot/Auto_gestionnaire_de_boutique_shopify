#include "elit21/orders/OrderRiskDecision.h"

namespace elit21::orders {

OrderRiskDecision::OrderRiskDecision()
    : platform::BusinessComponent(
          "OrderRiskDecision",
          "order validation and supplier placement workflow",
          platform::BusinessComponentSpec{
              "orders",
              "model_operation",
              std::vector<std::string>{},
              false,
              false,
              4U * 1024U * 1024U}) {}

platform::OperationResult OrderRiskDecision::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::orders
