#include "elit21/automation/AutomationRuleEngine.h"

namespace elit21::automation {

AutomationRuleEngine::AutomationRuleEngine()
    : platform::BusinessComponent(
          "AutomationRuleEngine",
          "autonomous policy and task orchestration",
          platform::BusinessComponentSpec{
              "automation",
              "orchestrate",
              std::vector<std::string>{},
              true,
              false,
              4U * 1024U * 1024U}) {}

platform::OperationResult AutomationRuleEngine::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::automation
