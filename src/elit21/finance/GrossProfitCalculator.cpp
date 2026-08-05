#include "elit21/finance/GrossProfitCalculator.h"

namespace elit21::finance {

GrossProfitCalculator::GrossProfitCalculator()
    : platform::BusinessComponent(
          "GrossProfitCalculator",
          "financial ledger and profitability reconciliation",
          platform::BusinessComponentSpec{
              "finance",
              "validate_transform",
              std::vector<std::string>{},
              false,
              false,
              4U * 1024U * 1024U}) {}

platform::OperationResult GrossProfitCalculator::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::finance
