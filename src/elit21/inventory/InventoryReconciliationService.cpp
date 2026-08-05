#include "elit21/inventory/InventoryReconciliationService.h"

namespace elit21::inventory {

InventoryReconciliationService::InventoryReconciliationService()
    : platform::BusinessComponent(
          "InventoryReconciliationService",
          "inventory synchronization and oversell prevention",
          platform::BusinessComponentSpec{
              "inventory",
              "orchestrate",
              std::vector<std::string>{},
              true,
              false,
              4U * 1024U * 1024U}) {}

platform::OperationResult InventoryReconciliationService::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::inventory
