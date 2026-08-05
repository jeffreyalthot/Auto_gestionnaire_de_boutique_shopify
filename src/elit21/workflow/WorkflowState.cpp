#include "elit21/workflow/WorkflowState.h"

namespace elit21::workflow {

WorkflowState::WorkflowState()
    : platform::BusinessComponent(
          "WorkflowState",
          "durable autonomous workflow and saga execution",
          platform::BusinessComponentSpec{
              "workflow",
              "orchestrate",
              std::vector<std::string>{},
              true,
              false,
              4U * 1024U * 1024U}) {}

platform::OperationResult WorkflowState::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::workflow
