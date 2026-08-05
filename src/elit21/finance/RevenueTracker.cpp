#include "elit21/finance/RevenueTracker.h"

namespace elit21::finance {

RevenueTracker::RevenueTracker()
    : platform::BusinessComponent(
          "RevenueTracker",
          "financial ledger and profitability reconciliation",
          platform::BusinessComponentSpec{
              "finance",
              "model_operation",
              std::vector<std::string>{},
              false,
              false,
              4U * 1024U * 1024U}) {}

platform::OperationResult RevenueTracker::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::finance
