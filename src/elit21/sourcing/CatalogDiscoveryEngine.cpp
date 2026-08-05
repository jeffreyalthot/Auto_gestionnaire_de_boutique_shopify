#include "elit21/sourcing/CatalogDiscoveryEngine.h"

namespace elit21::sourcing {

CatalogDiscoveryEngine::CatalogDiscoveryEngine()
    : platform::BusinessComponent(
          "CatalogDiscoveryEngine",
          "autonomous supplier and product sourcing",
          platform::BusinessComponentSpec{
              "sourcing",
              "orchestrate",
              std::vector<std::string>{},
              true,
              false,
              4U * 1024U * 1024U}) {}

platform::OperationResult CatalogDiscoveryEngine::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::sourcing
