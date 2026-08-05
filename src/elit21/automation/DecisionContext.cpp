#include "elit21/automation/DecisionContext.h"

namespace elit21::automation {

DecisionContext::DecisionContext()
    : platform::BusinessComponent(
          "DecisionContext",
          "autonomous policy and task orchestration",
          platform::BusinessComponentSpec{
              "automation",
              "model_operation",
              std::vector<std::string>{},
              false,
              false,
              4U * 1024U * 1024U}) {}

platform::OperationResult DecisionContext::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::automation
