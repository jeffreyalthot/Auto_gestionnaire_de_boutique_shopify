#include "elit21/sourcing/ProductRecommendationDiscovery.h"

namespace elit21::sourcing {

ProductRecommendationDiscovery::ProductRecommendationDiscovery()
    : platform::BusinessComponent(
          "ProductRecommendationDiscovery",
          "autonomous supplier and product sourcing",
          platform::BusinessComponentSpec{
              "sourcing",
              "model_operation",
              std::vector<std::string>{},
              false,
              false,
              4U * 1024U * 1024U}) {}

platform::OperationResult ProductRecommendationDiscovery::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::sourcing
