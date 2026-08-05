#include "elit21/repositories/TokenRepository.h"

namespace elit21::repositories {

TokenRepository::TokenRepository()
    : platform::BusinessComponent(
          "TokenRepository",
          "persistent repository access",
          platform::BusinessComponentSpec{
              "repositories",
              "model_operation",
              std::vector<std::string>{},
              false,
              false,
              4U * 1024U * 1024U}) {}

platform::OperationResult TokenRepository::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::repositories
