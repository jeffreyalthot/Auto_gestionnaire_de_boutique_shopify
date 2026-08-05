#include "elit21/inventory/AvailabilityDecision.h"

namespace elit21::inventory {

AvailabilityDecision::AvailabilityDecision()
    : platform::BusinessComponent(
          "AvailabilityDecision",
          "inventory synchronization and oversell prevention",
          platform::BusinessComponentSpec{
              "inventory",
              "model_operation",
              std::vector<std::string>{},
              false,
              false,
              4U * 1024U * 1024U}) {}

platform::OperationResult AvailabilityDecision::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::inventory
