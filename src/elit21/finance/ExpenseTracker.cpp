#include "elit21/finance/ExpenseTracker.h"

namespace elit21::finance {

ExpenseTracker::ExpenseTracker()
    : platform::BusinessComponent(
          "ExpenseTracker",
          "financial ledger and profitability reconciliation",
          platform::BusinessComponentSpec{
              "finance",
              "command",
              std::vector<std::string>{},
              true,
              false,
              4U * 1024U * 1024U}) {}

platform::OperationResult ExpenseTracker::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::finance
