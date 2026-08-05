#include "elit21/orders/OrderValidationService.h"

namespace elit21::orders {

OrderValidationService::OrderValidationService()
    : platform::BusinessComponent(
          "OrderValidationService",
          "order validation and supplier placement workflow",
          platform::BusinessComponentSpec{
              "orders",
              "orchestrate",
              std::vector<std::string>{},
              true,
              false,
              4U * 1024U * 1024U}) {}

platform::OperationResult OrderValidationService::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::orders
