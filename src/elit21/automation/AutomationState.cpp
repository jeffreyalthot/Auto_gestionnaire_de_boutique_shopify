#include "elit21/automation/AutomationState.h"

namespace elit21::automation {

AutomationState::AutomationState()
    : platform::BusinessComponent(
          "AutomationState",
          "autonomous policy and task orchestration",
          platform::BusinessComponentSpec{
              "automation",
              "model_operation",
              std::vector<std::string>{},
              false,
              false,
              4U * 1024U * 1024U}) {}

platform::OperationResult AutomationState::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::automation
