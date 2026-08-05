#include "elit21/automation/AutomationRule.h"

namespace elit21::automation {

AutomationRule::AutomationRule()
    : platform::BusinessComponent(
          "AutomationRule",
          "autonomous policy and task orchestration",
          platform::BusinessComponentSpec{
              "automation",
              "model_operation",
              std::vector<std::string>{},
              false,
              false,
              4U * 1024U * 1024U}) {}

platform::OperationResult AutomationRule::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::automation
