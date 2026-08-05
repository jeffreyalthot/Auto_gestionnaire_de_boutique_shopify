#include "elit21/finance/CostOfGoodsCalculator.h"

namespace elit21::finance {

CostOfGoodsCalculator::CostOfGoodsCalculator()
    : platform::BusinessComponent(
          "CostOfGoodsCalculator",
          "financial ledger and profitability reconciliation",
          platform::BusinessComponentSpec{
              "finance",
              "validate_transform",
              std::vector<std::string>{},
              false,
              false,
              4U * 1024U * 1024U}) {}

platform::OperationResult CostOfGoodsCalculator::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::finance
