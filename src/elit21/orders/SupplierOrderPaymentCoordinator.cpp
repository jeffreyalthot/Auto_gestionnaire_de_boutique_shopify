#include "elit21/orders/SupplierOrderPaymentCoordinator.h"

namespace elit21::orders {

SupplierOrderPaymentCoordinator::SupplierOrderPaymentCoordinator()
    : platform::BusinessComponent(
          "SupplierOrderPaymentCoordinator",
          "order validation and supplier placement workflow",
          platform::BusinessComponentSpec{
              "orders",
              "orchestrate",
              std::vector<std::string>{},
              true,
              false,
              4U * 1024U * 1024U}) {}

platform::OperationResult SupplierOrderPaymentCoordinator::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::orders
