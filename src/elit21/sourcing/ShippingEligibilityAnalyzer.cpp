#include "elit21/sourcing/ShippingEligibilityAnalyzer.h"

namespace elit21::sourcing {

ShippingEligibilityAnalyzer::ShippingEligibilityAnalyzer()
    : platform::BusinessComponent(
          "ShippingEligibilityAnalyzer",
          "autonomous supplier and product sourcing",
          platform::BusinessComponentSpec{
              "sourcing",
              "query",
              std::vector<std::string>{},
              false,
              false,
              4U * 1024U * 1024U}) {}

platform::OperationResult ShippingEligibilityAnalyzer::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::sourcing
