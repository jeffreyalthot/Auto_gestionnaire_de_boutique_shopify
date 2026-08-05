#include "elit21/workflow/WorkflowContext.h"

namespace elit21::workflow {

WorkflowContext::WorkflowContext()
    : platform::BusinessComponent(
          "WorkflowContext",
          "durable autonomous workflow and saga execution",
          platform::BusinessComponentSpec{
              "workflow",
              "orchestrate",
              std::vector<std::string>{},
              true,
              false,
              4U * 1024U * 1024U}) {}

platform::OperationResult WorkflowContext::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::workflow
