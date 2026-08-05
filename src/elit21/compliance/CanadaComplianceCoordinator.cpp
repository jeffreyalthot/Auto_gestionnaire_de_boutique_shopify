#include "elit21/compliance/CanadaComplianceCoordinator.h"

namespace elit21::compliance {

CanadaComplianceCoordinator::CanadaComplianceCoordinator()
    : platform::BusinessComponent(
          "CanadaComplianceCoordinator",
          "Canadian product, privacy and address compliance",
          platform::BusinessComponentSpec{
              "compliance",
              "orchestrate",
              std::vector<std::string>{},
              true,
              false,
              4U * 1024U * 1024U}) {}

platform::OperationResult CanadaComplianceCoordinator::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::compliance
