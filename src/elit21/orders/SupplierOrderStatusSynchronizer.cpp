#include "elit21/orders/SupplierOrderStatusSynchronizer.h"

namespace elit21::orders {

SupplierOrderStatusSynchronizer::SupplierOrderStatusSynchronizer()
    : platform::BusinessComponent(
          "SupplierOrderStatusSynchronizer",
          "order validation and supplier placement workflow",
          platform::BusinessComponentSpec{
              "orders",
              "command",
              std::vector<std::string>{"order_id"},
              true,
              false,
              4U * 1024U * 1024U}) {}

platform::OperationResult SupplierOrderStatusSynchronizer::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::orders
