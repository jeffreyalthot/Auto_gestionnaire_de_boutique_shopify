#include "elit21/automation/AutonomousManager.h"

namespace elit21::automation {

AutonomousManager::AutonomousManager()
    : platform::BusinessComponent(
          "AutonomousManager",
          "autonomous policy and task orchestration",
          platform::BusinessComponentSpec{
              "automation",
              "orchestrate",
              std::vector<std::string>{},
              true,
              false,
              4U * 1024U * 1024U}) {}

platform::OperationResult AutonomousManager::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::automation
