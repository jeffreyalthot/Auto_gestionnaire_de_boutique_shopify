#include "elit21/automation/AutomationCoordinator.h"

namespace elit21::automation {

AutomationCoordinator::AutomationCoordinator()
    : platform::BusinessComponent(
          "AutomationCoordinator",
          "autonomous policy and task orchestration",
          platform::BusinessComponentSpec{
              "automation",
              "orchestrate",
              std::vector<std::string>{},
              true,
              false,
              4U * 1024U * 1024U}) {}

platform::OperationResult AutomationCoordinator::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::automation
