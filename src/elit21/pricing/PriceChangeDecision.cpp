#include "elit21/pricing/PriceChangeDecision.h"

namespace elit21::pricing {

PriceChangeDecision::PriceChangeDecision()
    : platform::BusinessComponent(
          "PriceChangeDecision",
          "100 percent markup before shipping pricing enforcement",
          platform::BusinessComponentSpec{
              "pricing",
              "model_operation",
              std::vector<std::string>{},
              false,
              false,
              4U * 1024U * 1024U}) {}

platform::OperationResult PriceChangeDecision::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::pricing
