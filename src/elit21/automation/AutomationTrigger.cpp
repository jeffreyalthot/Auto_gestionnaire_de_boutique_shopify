#include "elit21/automation/AutomationTrigger.h"

namespace elit21::automation {

AutomationTrigger::AutomationTrigger()
    : platform::BusinessComponent(
          "AutomationTrigger",
          "autonomous policy and task orchestration",
          platform::BusinessComponentSpec{
              "automation",
              "model_operation",
              std::vector<std::string>{},
              false,
              false,
              4U * 1024U * 1024U}) {}

platform::OperationResult AutomationTrigger::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::automation
