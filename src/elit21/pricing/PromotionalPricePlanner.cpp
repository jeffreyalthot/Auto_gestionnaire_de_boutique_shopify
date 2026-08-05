#include "elit21/pricing/PromotionalPricePlanner.h"

namespace elit21::pricing {

PromotionalPricePlanner::PromotionalPricePlanner()
    : platform::BusinessComponent(
          "PromotionalPricePlanner",
          "100 percent markup before shipping pricing enforcement",
          platform::BusinessComponentSpec{
              "pricing",
              "model_operation",
              std::vector<std::string>{},
              false,
              false,
              4U * 1024U * 1024U}) {}

platform::OperationResult PromotionalPricePlanner::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::pricing
