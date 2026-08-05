#include "elit21/pricing/AliExpressCostCalculator.h"

namespace elit21::pricing {

AliExpressCostCalculator::AliExpressCostCalculator()
    : platform::BusinessComponent(
          "AliExpressCostCalculator",
          "100 percent markup before shipping pricing enforcement",
          platform::BusinessComponentSpec{
              "pricing",
              "validate_transform",
              std::vector<std::string>{},
              false,
              false,
              4U * 1024U * 1024U}) {}

platform::OperationResult AliExpressCostCalculator::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::pricing
