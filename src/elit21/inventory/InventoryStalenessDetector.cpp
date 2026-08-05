#include "elit21/inventory/InventoryStalenessDetector.h"

namespace elit21::inventory {

InventoryStalenessDetector::InventoryStalenessDetector()
    : platform::BusinessComponent(
          "InventoryStalenessDetector",
          "inventory synchronization and oversell prevention",
          platform::BusinessComponentSpec{
              "inventory",
              "query",
              std::vector<std::string>{},
              false,
              false,
              4U * 1024U * 1024U}) {}

platform::OperationResult InventoryStalenessDetector::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::inventory
