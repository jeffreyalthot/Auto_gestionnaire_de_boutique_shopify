#include "elit21/compliance/ProductComplianceDecision.h"

namespace elit21::compliance {

ProductComplianceDecision::ProductComplianceDecision()
    : platform::BusinessComponent(
          "ProductComplianceDecision",
          "Canadian product, privacy and address compliance",
          platform::BusinessComponentSpec{
              "compliance",
              "model_operation",
              std::vector<std::string>{},
              false,
              false,
              4U * 1024U * 1024U}) {}

platform::OperationResult ProductComplianceDecision::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::compliance
