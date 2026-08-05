#include "elit21/workflow/WorkflowDefinition.h"

namespace elit21::workflow {

WorkflowDefinition::WorkflowDefinition()
    : platform::BusinessComponent(
          "WorkflowDefinition",
          "durable autonomous workflow and saga execution",
          platform::BusinessComponentSpec{
              "workflow",
              "orchestrate",
              std::vector<std::string>{},
              true,
              false,
              4U * 1024U * 1024U}) {}

platform::OperationResult WorkflowDefinition::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::workflow
