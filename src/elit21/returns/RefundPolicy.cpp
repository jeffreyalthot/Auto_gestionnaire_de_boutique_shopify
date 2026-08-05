#include "elit21/returns/RefundPolicy.h"

namespace elit21::returns {

RefundPolicy::RefundPolicy()
    : platform::BusinessComponent(
          "RefundPolicy",
          "return, refund and dispute coordination",
          platform::BusinessComponentSpec{
              "returns",
              "validate_transform",
              std::vector<std::string>{"order_id"},
              false,
              false,
              4U * 1024U * 1024U}) {}

platform::OperationResult RefundPolicy::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::returns
