#include "elit21/orders/OrderReconciliationService.h"

namespace elit21::orders {

OrderReconciliationService::OrderReconciliationService()
    : platform::BusinessComponent(
          "OrderReconciliationService",
          "order validation and supplier placement workflow",
          platform::BusinessComponentSpec{
              "orders",
              "orchestrate",
              std::vector<std::string>{},
              true,
              false,
              4U * 1024U * 1024U}) {}

platform::OperationResult OrderReconciliationService::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::orders
