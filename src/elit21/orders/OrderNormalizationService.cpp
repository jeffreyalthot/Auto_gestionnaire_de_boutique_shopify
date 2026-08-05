#include "elit21/orders/OrderNormalizationService.h"

namespace elit21::orders {

OrderNormalizationService::OrderNormalizationService()
    : platform::BusinessComponent(
          "OrderNormalizationService",
          "order validation and supplier placement workflow",
          platform::BusinessComponentSpec{
              "orders",
              "orchestrate",
              std::vector<std::string>{},
              true,
              false,
              4U * 1024U * 1024U}) {}

platform::OperationResult OrderNormalizationService::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::orders
