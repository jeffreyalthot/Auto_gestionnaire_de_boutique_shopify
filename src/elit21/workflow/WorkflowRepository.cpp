#include "elit21/workflow/WorkflowRepository.h"

namespace elit21::workflow {

WorkflowRepository::WorkflowRepository()
    : platform::BusinessComponent(
          "WorkflowRepository",
          "durable autonomous workflow and saga execution",
          platform::BusinessComponentSpec{
              "workflow",
              "orchestrate",
              std::vector<std::string>{},
              true,
              false,
              4U * 1024U * 1024U}) {}

platform::OperationResult WorkflowRepository::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::workflow
