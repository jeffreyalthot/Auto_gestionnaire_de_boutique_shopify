#include "elit21/inventory/InventoryCoordinator.h"

namespace elit21::inventory {

InventoryCoordinator::InventoryCoordinator()
    : platform::BusinessComponent(
          "InventoryCoordinator",
          "inventory synchronization and oversell prevention",
          platform::BusinessComponentSpec{
              "inventory",
              "orchestrate",
              std::vector<std::string>{},
              true,
              false,
              4U * 1024U * 1024U}) {}

platform::OperationResult InventoryCoordinator::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::inventory
