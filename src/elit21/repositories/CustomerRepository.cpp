#include "elit21/repositories/CustomerRepository.h"

namespace elit21::repositories {

CustomerRepository::CustomerRepository()
    : platform::BusinessComponent(
          "CustomerRepository",
          "persistent repository access",
          platform::BusinessComponentSpec{
              "repositories",
              "model_operation",
              std::vector<std::string>{},
              false,
              false,
              4U * 1024U * 1024U}) {}

platform::OperationResult CustomerRepository::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::repositories
