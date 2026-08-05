#include "elit21/orders/OrderCaptureService.h"

namespace elit21::orders {

OrderCaptureService::OrderCaptureService()
    : platform::BusinessComponent(
          "OrderCaptureService",
          "order validation and supplier placement workflow",
          platform::BusinessComponentSpec{
              "orders",
              "orchestrate",
              std::vector<std::string>{},
              true,
              false,
              4U * 1024U * 1024U}) {}

platform::OperationResult OrderCaptureService::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::orders
