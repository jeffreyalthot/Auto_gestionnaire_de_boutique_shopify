#include "elit21/pricing/FeeCalculator.h"

namespace elit21::pricing {

FeeCalculator::FeeCalculator()
    : platform::BusinessComponent(
          "FeeCalculator",
          "100 percent markup before shipping pricing enforcement",
          platform::BusinessComponentSpec{
              "pricing",
              "validate_transform",
              std::vector<std::string>{},
              false,
              false,
              4U * 1024U * 1024U}) {}

platform::OperationResult FeeCalculator::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::pricing
