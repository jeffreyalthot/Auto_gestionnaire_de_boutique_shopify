#include "elit21/inventory/VariantAvailabilityService.h"

namespace elit21::inventory {

VariantAvailabilityService::VariantAvailabilityService()
    : platform::BusinessComponent(
          "VariantAvailabilityService",
          "inventory synchronization and oversell prevention",
          platform::BusinessComponentSpec{
              "inventory",
              "orchestrate",
              std::vector<std::string>{},
              true,
              false,
              4U * 1024U * 1024U}) {}

platform::OperationResult VariantAvailabilityService::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::inventory
