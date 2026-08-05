#include "elit21/returns/ReturnEligibilityChecker.h"

namespace elit21::returns {

ReturnEligibilityChecker::ReturnEligibilityChecker()
    : platform::BusinessComponent(
          "ReturnEligibilityChecker",
          "return, refund and dispute coordination",
          platform::BusinessComponentSpec{
              "returns",
              "validate_transform",
              std::vector<std::string>{"order_id"},
              false,
              false,
              4U * 1024U * 1024U}) {}

platform::OperationResult ReturnEligibilityChecker::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::returns
