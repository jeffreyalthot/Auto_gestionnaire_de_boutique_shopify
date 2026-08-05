#include "elit21/repositories/OutboxRepository.h"

namespace elit21::repositories {

OutboxRepository::OutboxRepository()
    : platform::BusinessComponent(
          "OutboxRepository",
          "persistent repository access",
          platform::BusinessComponentSpec{
              "repositories",
              "model_operation",
              std::vector<std::string>{},
              false,
              false,
              4U * 1024U * 1024U}) {}

platform::OperationResult OutboxRepository::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::repositories
