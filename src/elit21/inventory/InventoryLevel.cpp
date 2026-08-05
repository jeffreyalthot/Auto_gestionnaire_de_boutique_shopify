#include "elit21/inventory/InventoryLevel.h"

namespace elit21::inventory {

InventoryLevel::InventoryLevel()
    : platform::BusinessComponent(
          "InventoryLevel",
          "inventory synchronization and oversell prevention",
          platform::BusinessComponentSpec{
              "inventory",
              "model_operation",
              std::vector<std::string>{},
              false,
              false,
              4U * 1024U * 1024U}) {}

platform::OperationResult InventoryLevel::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::inventory
