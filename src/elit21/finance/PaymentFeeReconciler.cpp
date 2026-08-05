#include "elit21/finance/PaymentFeeReconciler.h"

namespace elit21::finance {

PaymentFeeReconciler::PaymentFeeReconciler()
    : platform::BusinessComponent(
          "PaymentFeeReconciler",
          "financial ledger and profitability reconciliation",
          platform::BusinessComponentSpec{
              "finance",
              "command",
              std::vector<std::string>{},
              true,
              false,
              4U * 1024U * 1024U}) {}

platform::OperationResult PaymentFeeReconciler::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::finance
