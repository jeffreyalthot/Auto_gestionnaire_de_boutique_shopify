#include "elit21/automation/AutomationEvent.h"

namespace elit21::automation {

AutomationEvent::AutomationEvent()
    : platform::BusinessComponent(
          "AutomationEvent",
          "autonomous policy and task orchestration",
          platform::BusinessComponentSpec{
              "automation",
              "model_operation",
              std::vector<std::string>{},
              false,
              false,
              4U * 1024U * 1024U}) {}

platform::OperationResult AutomationEvent::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::automation
