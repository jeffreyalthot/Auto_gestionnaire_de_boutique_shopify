#include "elit21/repositories/SupplierRepository.h"

namespace elit21::repositories {

SupplierRepository::SupplierRepository()
    : platform::BusinessComponent(
          "SupplierRepository",
          "persistent repository access",
          platform::BusinessComponentSpec{
              "repositories",
              "model_operation",
              std::vector<std::string>{},
              false,
              false,
              4U * 1024U * 1024U}) {}

platform::OperationResult SupplierRepository::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::repositories
