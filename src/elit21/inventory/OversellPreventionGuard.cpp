#include "elit21/inventory/OversellPreventionGuard.h"

namespace elit21::inventory {

OversellPreventionGuard::OversellPreventionGuard()
    : platform::BusinessComponent(
          "OversellPreventionGuard",
          "inventory synchronization and oversell prevention",
          platform::BusinessComponentSpec{
              "inventory",
              "validate_transform",
              std::vector<std::string>{},
              false,
              false,
              4U * 1024U * 1024U}) {}

platform::OperationResult OversellPreventionGuard::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::inventory
