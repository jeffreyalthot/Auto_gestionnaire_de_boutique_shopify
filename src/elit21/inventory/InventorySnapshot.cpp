#include "elit21/inventory/InventorySnapshot.h"

namespace elit21::inventory {

InventorySnapshot::InventorySnapshot()
    : platform::BusinessComponent(
          "InventorySnapshot",
          "inventory synchronization and oversell prevention",
          platform::BusinessComponentSpec{
              "inventory",
              "query",
              std::vector<std::string>{},
              false,
              false,
              4U * 1024U * 1024U}) {}

platform::OperationResult InventorySnapshot::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::inventory
