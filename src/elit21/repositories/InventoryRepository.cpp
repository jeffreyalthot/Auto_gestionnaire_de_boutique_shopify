#include "elit21/repositories/InventoryRepository.h"

namespace elit21::repositories {

InventoryRepository::InventoryRepository()
    : platform::BusinessComponent(
          "InventoryRepository",
          "persistent repository access",
          platform::BusinessComponentSpec{
              "repositories",
              "model_operation",
              std::vector<std::string>{},
              false,
              false,
              4U * 1024U * 1024U}) {}

platform::OperationResult InventoryRepository::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::repositories
