#include "elit21/repositories/ProductRepository.h"

namespace elit21::repositories {

ProductRepository::ProductRepository()
    : platform::BusinessComponent(
          "ProductRepository",
          "persistent repository access",
          platform::BusinessComponentSpec{
              "repositories",
              "model_operation",
              std::vector<std::string>{},
              false,
              false,
              4U * 1024U * 1024U}) {}

platform::OperationResult ProductRepository::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::repositories
