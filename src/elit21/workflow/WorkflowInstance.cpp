#include "elit21/workflow/WorkflowInstance.h"

namespace elit21::workflow {

WorkflowInstance::WorkflowInstance()
    : platform::BusinessComponent(
          "WorkflowInstance",
          "durable autonomous workflow and saga execution",
          platform::BusinessComponentSpec{
              "workflow",
              "orchestrate",
              std::vector<std::string>{},
              true,
              false,
              4U * 1024U * 1024U}) {}

platform::OperationResult WorkflowInstance::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::workflow
