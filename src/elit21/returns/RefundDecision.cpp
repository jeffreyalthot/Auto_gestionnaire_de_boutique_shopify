#include "elit21/returns/RefundDecision.h"

namespace elit21::returns {

RefundDecision::RefundDecision()
    : platform::BusinessComponent(
          "RefundDecision",
          "return, refund and dispute coordination",
          platform::BusinessComponentSpec{
              "returns",
              "command",
              std::vector<std::string>{"order_id"},
              true,
              false,
              4U * 1024U * 1024U}) {}

platform::OperationResult RefundDecision::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::returns
