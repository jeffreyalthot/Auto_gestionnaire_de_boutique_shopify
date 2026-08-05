#include "elit21/sourcing/SourcingCoordinator.h"

namespace elit21::sourcing {

SourcingCoordinator::SourcingCoordinator()
    : platform::BusinessComponent(
          "SourcingCoordinator",
          "autonomous supplier and product sourcing",
          platform::BusinessComponentSpec{
              "sourcing",
              "orchestrate",
              std::vector<std::string>{},
              true,
              false,
              4U * 1024U * 1024U}) {}

platform::OperationResult SourcingCoordinator::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::sourcing
