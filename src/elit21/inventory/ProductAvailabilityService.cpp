#include "elit21/inventory/ProductAvailabilityService.h"

namespace elit21::inventory {

ProductAvailabilityService::ProductAvailabilityService()
    : platform::BusinessComponent(
          "ProductAvailabilityService",
          "inventory synchronization and oversell prevention",
          platform::BusinessComponentSpec{
              "inventory",
              "orchestrate",
              std::vector<std::string>{},
              true,
              false,
              4U * 1024U * 1024U}) {}

platform::OperationResult ProductAvailabilityService::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::inventory
