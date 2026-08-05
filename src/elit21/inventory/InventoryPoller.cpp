#include "elit21/inventory/InventoryPoller.h"

namespace elit21::inventory {

InventoryPoller::InventoryPoller()
    : platform::BusinessComponent(
          "InventoryPoller",
          "inventory synchronization and oversell prevention",
          platform::BusinessComponentSpec{
              "inventory",
              "model_operation",
              std::vector<std::string>{},
              false,
              false,
              4U * 1024U * 1024U}) {}

platform::OperationResult InventoryPoller::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::inventory
