#include "elit21/pricing/FreightCostCalculator.h"

namespace elit21::pricing {

FreightCostCalculator::FreightCostCalculator()
    : platform::BusinessComponent(
          "FreightCostCalculator",
          "100 percent markup before shipping pricing enforcement",
          platform::BusinessComponentSpec{
              "pricing",
              "validate_transform",
              std::vector<std::string>{},
              false,
              false,
              4U * 1024U * 1024U}) {}

platform::OperationResult FreightCostCalculator::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::pricing
