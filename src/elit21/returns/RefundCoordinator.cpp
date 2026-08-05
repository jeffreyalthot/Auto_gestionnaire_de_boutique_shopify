#include "elit21/returns/RefundCoordinator.h"

namespace elit21::returns {

RefundCoordinator::RefundCoordinator()
    : platform::BusinessComponent(
          "RefundCoordinator",
          "return, refund and dispute coordination",
          platform::BusinessComponentSpec{
              "returns",
              "command",
              std::vector<std::string>{"order_id"},
              true,
              false,
              4U * 1024U * 1024U}) {}

platform::OperationResult RefundCoordinator::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::returns
