#include "elit21/inventory/InventorySafetyBuffer.h"

namespace elit21::inventory {

InventorySafetyBuffer::InventorySafetyBuffer()
    : platform::BusinessComponent(
          "InventorySafetyBuffer",
          "inventory synchronization and oversell prevention",
          platform::BusinessComponentSpec{
              "inventory",
              "model_operation",
              std::vector<std::string>{},
              false,
              false,
              4U * 1024U * 1024U}) {}

platform::OperationResult InventorySafetyBuffer::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::inventory
