#include "elit21/finance/NetProfitCalculator.h"

namespace elit21::finance {

NetProfitCalculator::NetProfitCalculator()
    : platform::BusinessComponent(
          "NetProfitCalculator",
          "financial ledger and profitability reconciliation",
          platform::BusinessComponentSpec{
              "finance",
              "validate_transform",
              std::vector<std::string>{},
              false,
              false,
              4U * 1024U * 1024U}) {}

platform::OperationResult NetProfitCalculator::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::finance
