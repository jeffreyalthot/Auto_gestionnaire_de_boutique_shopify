#include "elit21/finance/PayoutReconciliationService.h"

namespace elit21::finance {

PayoutReconciliationService::PayoutReconciliationService()
    : platform::BusinessComponent(
          "PayoutReconciliationService",
          "financial ledger and profitability reconciliation",
          platform::BusinessComponentSpec{
              "finance",
              "orchestrate",
              std::vector<std::string>{},
              true,
              false,
              4U * 1024U * 1024U}) {}

platform::OperationResult PayoutReconciliationService::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::finance
