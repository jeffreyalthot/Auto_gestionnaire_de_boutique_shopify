#include "elit21/finance/FinancialReconciliationService.h"

namespace elit21::finance {

FinancialReconciliationService::FinancialReconciliationService()
    : platform::BusinessComponent(
          "FinancialReconciliationService",
          "financial ledger and profitability reconciliation",
          platform::BusinessComponentSpec{
              "finance",
              "orchestrate",
              std::vector<std::string>{},
              true,
              false,
              4U * 1024U * 1024U}) {}

platform::OperationResult FinancialReconciliationService::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::finance
