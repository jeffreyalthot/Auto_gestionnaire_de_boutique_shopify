#include "elit21/repositories/TaskRepository.h"

namespace elit21::repositories {

TaskRepository::TaskRepository()
    : platform::BusinessComponent(
          "TaskRepository",
          "persistent repository access",
          platform::BusinessComponentSpec{
              "repositories",
              "model_operation",
              std::vector<std::string>{},
              false,
              false,
              4U * 1024U * 1024U}) {}

platform::OperationResult TaskRepository::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::repositories
