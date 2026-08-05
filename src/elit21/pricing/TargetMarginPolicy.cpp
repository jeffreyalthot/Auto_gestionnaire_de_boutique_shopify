#include "elit21/pricing/TargetMarginPolicy.h"

namespace elit21::pricing {

TargetMarginPolicy::TargetMarginPolicy()
    : platform::BusinessComponent(
          "TargetMarginPolicy",
          "100 percent markup before shipping pricing enforcement",
          platform::BusinessComponentSpec{
              "pricing",
              "validate_transform",
              std::vector<std::string>{"supplier_cost_cad"},
              false,
              false,
              4U * 1024U * 1024U}) {}

platform::OperationResult TargetMarginPolicy::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::pricing
