#include "elit21/orders/FreightSelectionPolicy.h"

namespace elit21::orders {

FreightSelectionPolicy::FreightSelectionPolicy()
    : platform::BusinessComponent(
          "FreightSelectionPolicy",
          "order validation and supplier placement workflow",
          platform::BusinessComponentSpec{
              "orders",
              "validate_transform",
              std::vector<std::string>{},
              false,
              false,
              4U * 1024U * 1024U}) {}

platform::OperationResult FreightSelectionPolicy::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::orders
