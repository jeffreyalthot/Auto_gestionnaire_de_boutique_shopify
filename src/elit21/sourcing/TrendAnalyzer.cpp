#include "elit21/sourcing/TrendAnalyzer.h"

namespace elit21::sourcing {

TrendAnalyzer::TrendAnalyzer()
    : platform::BusinessComponent(
          "TrendAnalyzer",
          "autonomous supplier and product sourcing",
          platform::BusinessComponentSpec{
              "sourcing",
              "query",
              std::vector<std::string>{},
              false,
              false,
              4U * 1024U * 1024U}) {}

platform::OperationResult TrendAnalyzer::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::sourcing
