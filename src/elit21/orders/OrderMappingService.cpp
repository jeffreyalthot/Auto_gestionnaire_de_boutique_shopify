#include "elit21/orders/OrderMappingService.h"

namespace elit21::orders {

OrderMappingService::OrderMappingService()
    : platform::BusinessComponent(
          "OrderMappingService",
          "order validation and supplier placement workflow",
          platform::BusinessComponentSpec{
              "orders",
              "orchestrate",
              std::vector<std::string>{},
              true,
              false,
              4U * 1024U * 1024U}) {}

platform::OperationResult OrderMappingService::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::orders
