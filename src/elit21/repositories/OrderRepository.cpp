#include "elit21/repositories/OrderRepository.h"

namespace elit21::repositories {

OrderRepository::OrderRepository()
    : platform::BusinessComponent(
          "OrderRepository",
          "persistent repository access",
          platform::BusinessComponentSpec{
              "repositories",
              "model_operation",
              std::vector<std::string>{},
              false,
              false,
              4U * 1024U * 1024U}) {}

platform::OperationResult OrderRepository::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::repositories
