#include "elit21/workflow/WorkflowScheduler.h"

namespace elit21::workflow {

WorkflowScheduler::WorkflowScheduler()
    : platform::BusinessComponent(
          "WorkflowScheduler",
          "durable autonomous workflow and saga execution",
          platform::BusinessComponentSpec{
              "workflow",
              "command",
              std::vector<std::string>{},
              true,
              false,
              4U * 1024U * 1024U}) {}

platform::OperationResult WorkflowScheduler::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::workflow
