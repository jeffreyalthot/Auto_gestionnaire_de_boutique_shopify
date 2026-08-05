#include "elit21/repositories/CustomerMessageRepository.h"

namespace elit21::repositories {

CustomerMessageRepository::CustomerMessageRepository()
    : platform::BusinessComponent(
          "CustomerMessageRepository",
          "persistent repository access",
          platform::BusinessComponentSpec{
              "repositories",
              "model_operation",
              std::vector<std::string>{},
              false,
              false,
              4U * 1024U * 1024U}) {}

platform::OperationResult CustomerMessageRepository::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::repositories
