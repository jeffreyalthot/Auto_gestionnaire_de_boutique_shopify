#include "elit21/sourcing/CompetitionAnalyzer.h"

namespace elit21::sourcing {

CompetitionAnalyzer::CompetitionAnalyzer()
    : platform::BusinessComponent(
          "CompetitionAnalyzer",
          "autonomous supplier and product sourcing",
          platform::BusinessComponentSpec{
              "sourcing",
              "query",
              std::vector<std::string>{},
              false,
              false,
              4U * 1024U * 1024U}) {}

platform::OperationResult CompetitionAnalyzer::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::sourcing
