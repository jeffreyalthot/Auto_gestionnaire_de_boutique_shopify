#include "elit21/automation/AutonomousPolicy.h"

namespace elit21::automation {

AutonomousPolicy::AutonomousPolicy()
    : platform::BusinessComponent(
          "AutonomousPolicy",
          "autonomous policy and task orchestration",
          platform::BusinessComponentSpec{
              "automation",
              "validate_transform",
              std::vector<std::string>{},
              false,
              false,
              4U * 1024U * 1024U}) {}

platform::OperationResult AutonomousPolicy::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::automation
