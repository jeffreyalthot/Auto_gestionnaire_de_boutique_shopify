#include "elit21/workflow/WorkflowEngine.h"

namespace elit21::workflow {

WorkflowEngine::WorkflowEngine()
    : platform::BusinessComponent(
          "WorkflowEngine",
          "durable autonomous workflow and saga execution",
          platform::BusinessComponentSpec{
              "workflow",
              "orchestrate",
              std::vector<std::string>{},
              true,
              false,
              4U * 1024U * 1024U}) {}

platform::OperationResult WorkflowEngine::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::workflow
