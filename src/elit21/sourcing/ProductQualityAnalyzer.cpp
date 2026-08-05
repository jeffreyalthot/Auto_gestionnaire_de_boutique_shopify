#include "elit21/sourcing/ProductQualityAnalyzer.h"

namespace elit21::sourcing {

ProductQualityAnalyzer::ProductQualityAnalyzer()
    : platform::BusinessComponent(
          "ProductQualityAnalyzer",
          "autonomous supplier and product sourcing",
          platform::BusinessComponentSpec{
              "sourcing",
              "query",
              std::vector<std::string>{},
              false,
              false,
              4U * 1024U * 1024U}) {}

platform::OperationResult ProductQualityAnalyzer::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::sourcing
