#include "elit21/inventory/InventorySynchronizationService.h"

namespace elit21::inventory {

InventorySynchronizationService::InventorySynchronizationService()
    : platform::BusinessComponent(
          "InventorySynchronizationService",
          "inventory synchronization and oversell prevention",
          platform::BusinessComponentSpec{
              "inventory",
              "command",
              std::vector<std::string>{"sku"},
              true,
              false,
              4U * 1024U * 1024U}) {}

platform::OperationResult InventorySynchronizationService::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::inventory
