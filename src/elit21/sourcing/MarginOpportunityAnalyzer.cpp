#include "elit21/sourcing/MarginOpportunityAnalyzer.h"

namespace elit21::sourcing {

MarginOpportunityAnalyzer::MarginOpportunityAnalyzer()
    : platform::BusinessComponent(
          "MarginOpportunityAnalyzer",
          "autonomous supplier and product sourcing",
          platform::BusinessComponentSpec{
              "sourcing",
              "query",
              std::vector<std::string>{},
              false,
              false,
              4U * 1024U * 1024U}) {}

platform::OperationResult MarginOpportunityAnalyzer::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::sourcing
