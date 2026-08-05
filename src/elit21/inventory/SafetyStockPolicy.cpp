#include "elit21/inventory/SafetyStockPolicy.h"

namespace elit21::inventory {

SafetyStockPolicy::SafetyStockPolicy()
    : platform::BusinessComponent(
          "SafetyStockPolicy",
          "inventory synchronization and oversell prevention",
          platform::BusinessComponentSpec{
              "inventory",
              "validate_transform",
              std::vector<std::string>{"sku"},
              false,
              false,
              4U * 1024U * 1024U}) {}

platform::OperationResult SafetyStockPolicy::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::inventory
