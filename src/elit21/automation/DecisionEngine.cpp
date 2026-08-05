#include "elit21/automation/DecisionEngine.h"

namespace elit21::automation {

DecisionEngine::DecisionEngine()
    : platform::BusinessComponent(
          "DecisionEngine",
          "autonomous policy and task orchestration",
          platform::BusinessComponentSpec{
              "automation",
              "orchestrate",
              std::vector<std::string>{},
              true,
              false,
              4U * 1024U * 1024U}) {}

platform::OperationResult DecisionEngine::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::automation
