#include "elit21/repositories/DeadLetterRepository.h"

namespace elit21::repositories {

DeadLetterRepository::DeadLetterRepository()
    : platform::BusinessComponent(
          "DeadLetterRepository",
          "persistent repository access",
          platform::BusinessComponentSpec{
              "repositories",
              "model_operation",
              std::vector<std::string>{},
              false,
              false,
              4U * 1024U * 1024U}) {}

platform::OperationResult DeadLetterRepository::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::repositories
