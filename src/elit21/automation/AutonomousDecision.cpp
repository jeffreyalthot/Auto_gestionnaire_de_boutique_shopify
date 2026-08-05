#include "elit21/automation/AutonomousDecision.h"

namespace elit21::automation {

AutonomousDecision::AutonomousDecision()
    : platform::BusinessComponent(
          "AutonomousDecision",
          "autonomous policy and task orchestration",
          platform::BusinessComponentSpec{
              "automation",
              "model_operation",
              std::vector<std::string>{},
              false,
              false,
              4U * 1024U * 1024U}) {}

platform::OperationResult AutonomousDecision::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::automation
