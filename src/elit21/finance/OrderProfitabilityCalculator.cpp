#include "elit21/finance/OrderProfitabilityCalculator.h"

namespace elit21::finance {

OrderProfitabilityCalculator::OrderProfitabilityCalculator()
    : platform::BusinessComponent(
          "OrderProfitabilityCalculator",
          "financial ledger and profitability reconciliation",
          platform::BusinessComponentSpec{
              "finance",
              "validate_transform",
              std::vector<std::string>{"order_id"},
              false,
              false,
              4U * 1024U * 1024U}) {}

platform::OperationResult OrderProfitabilityCalculator::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::finance
