#include "elit21/sourcing/ProductRiskAnalyzer.h"

namespace elit21::sourcing {

ProductRiskAnalyzer::ProductRiskAnalyzer()
    : platform::BusinessComponent(
          "ProductRiskAnalyzer",
          "autonomous supplier and product sourcing",
          platform::BusinessComponentSpec{
              "sourcing",
              "query",
              std::vector<std::string>{},
              false,
              false,
              4U * 1024U * 1024U}) {}

platform::OperationResult ProductRiskAnalyzer::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::sourcing
