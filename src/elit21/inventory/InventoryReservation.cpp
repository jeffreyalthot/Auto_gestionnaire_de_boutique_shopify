#include "elit21/inventory/InventoryReservation.h"

namespace elit21::inventory {

InventoryReservation::InventoryReservation()
    : platform::BusinessComponent(
          "InventoryReservation",
          "inventory synchronization and oversell prevention",
          platform::BusinessComponentSpec{
              "inventory",
              "model_operation",
              std::vector<std::string>{},
              false,
              false,
              4U * 1024U * 1024U}) {}

platform::OperationResult InventoryReservation::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::inventory
