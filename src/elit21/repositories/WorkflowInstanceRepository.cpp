#include "elit21/repositories/WorkflowInstanceRepository.h"

namespace elit21::repositories {

WorkflowInstanceRepository::WorkflowInstanceRepository()
    : platform::BusinessComponent(
          "WorkflowInstanceRepository",
          "persistent repository access",
          platform::BusinessComponentSpec{
              "repositories",
              "orchestrate",
              std::vector<std::string>{},
              true,
              false,
              4U * 1024U * 1024U}) {}

platform::OperationResult WorkflowInstanceRepository::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::repositories
