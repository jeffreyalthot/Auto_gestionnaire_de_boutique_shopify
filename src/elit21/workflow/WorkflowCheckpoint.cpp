#include "elit21/workflow/WorkflowCheckpoint.h"

namespace elit21::workflow {

WorkflowCheckpoint::WorkflowCheckpoint()
    : platform::BusinessComponent(
          "WorkflowCheckpoint",
          "durable autonomous workflow and saga execution",
          platform::BusinessComponentSpec{
              "workflow",
              "orchestrate",
              std::vector<std::string>{},
              true,
              false,
              4U * 1024U * 1024U}) {}

platform::OperationResult WorkflowCheckpoint::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::workflow
