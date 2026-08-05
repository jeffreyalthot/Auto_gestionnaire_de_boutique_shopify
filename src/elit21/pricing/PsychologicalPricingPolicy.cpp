#include "elit21/pricing/PsychologicalPricingPolicy.h"

namespace elit21::pricing {

PsychologicalPricingPolicy::PsychologicalPricingPolicy()
    : platform::BusinessComponent(
          "PsychologicalPricingPolicy",
          "100 percent markup before shipping pricing enforcement",
          platform::BusinessComponentSpec{
              "pricing",
              "validate_transform",
              std::vector<std::string>{"supplier_cost_cad"},
              false,
              false,
              4U * 1024U * 1024U}) {}

platform::OperationResult PsychologicalPricingPolicy::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::pricing
