#include "elit21/repositories/SupplierOrderRepository.h"

namespace elit21::repositories {

SupplierOrderRepository::SupplierOrderRepository()
    : platform::BusinessComponent(
          "SupplierOrderRepository",
          "persistent repository access",
          platform::BusinessComponentSpec{
              "repositories",
              "model_operation",
              std::vector<std::string>{},
              false,
              false,
              4U * 1024U * 1024U}) {}

platform::OperationResult SupplierOrderRepository::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::repositories
