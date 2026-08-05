#include "elit21/workflow/SagaCoordinator.h"

namespace elit21::workflow {

SagaCoordinator::SagaCoordinator()
    : platform::BusinessComponent(
          "SagaCoordinator",
          "durable autonomous workflow and saga execution",
          platform::BusinessComponentSpec{
              "workflow",
              "orchestrate",
              std::vector<std::string>{},
              true,
              false,
              4U * 1024U * 1024U}) {}

platform::OperationResult SagaCoordinator::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::workflow
