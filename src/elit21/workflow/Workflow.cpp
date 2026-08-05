#include "elit21/workflow/Workflow.h"

namespace elit21::workflow {

Workflow::Workflow()
    : platform::BusinessComponent(
          "Workflow",
          "durable autonomous workflow and saga execution",
          platform::BusinessComponentSpec{
              "workflow",
              "orchestrate",
              std::vector<std::string>{},
              true,
              false,
              4U * 1024U * 1024U}) {}

platform::OperationResult Workflow::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::workflow
