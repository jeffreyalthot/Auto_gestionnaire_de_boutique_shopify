#include "elit21/automation/AutomationAction.h"

namespace elit21::automation {

AutomationAction::AutomationAction()
    : platform::BusinessComponent(
          "AutomationAction",
          "autonomous policy and task orchestration",
          platform::BusinessComponentSpec{
              "automation",
              "model_operation",
              std::vector<std::string>{},
              false,
              false,
              4U * 1024U * 1024U}) {}

platform::OperationResult AutomationAction::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::automation
