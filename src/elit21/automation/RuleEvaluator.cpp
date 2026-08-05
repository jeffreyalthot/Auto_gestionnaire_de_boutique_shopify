#include "elit21/automation/RuleEvaluator.h"

namespace elit21::automation {

RuleEvaluator::RuleEvaluator()
    : platform::BusinessComponent(
          "RuleEvaluator",
          "autonomous policy and task orchestration",
          platform::BusinessComponentSpec{
              "automation",
              "model_operation",
              std::vector<std::string>{},
              false,
              false,
              4U * 1024U * 1024U}) {}

platform::OperationResult RuleEvaluator::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::automation
