#include "elit21/finance/FinancialLedger.h"

namespace elit21::finance {

FinancialLedger::FinancialLedger()
    : platform::BusinessComponent(
          "FinancialLedger",
          "financial ledger and profitability reconciliation",
          platform::BusinessComponentSpec{
              "finance",
              "model_operation",
              std::vector<std::string>{},
              false,
              false,
              4U * 1024U * 1024U}) {}

platform::OperationResult FinancialLedger::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::finance
