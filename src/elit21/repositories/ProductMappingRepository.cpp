#include "elit21/repositories/ProductMappingRepository.h"

namespace elit21::repositories {

ProductMappingRepository::ProductMappingRepository()
    : platform::BusinessComponent(
          "ProductMappingRepository",
          "persistent repository access",
          platform::BusinessComponentSpec{
              "repositories",
              "model_operation",
              std::vector<std::string>{},
              false,
              false,
              4U * 1024U * 1024U}) {}

platform::OperationResult ProductMappingRepository::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::repositories
