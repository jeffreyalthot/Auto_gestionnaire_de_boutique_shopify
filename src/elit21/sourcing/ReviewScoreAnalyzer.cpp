#include "elit21/sourcing/ReviewScoreAnalyzer.h"

namespace elit21::sourcing {

ReviewScoreAnalyzer::ReviewScoreAnalyzer()
    : platform::BusinessComponent(
          "ReviewScoreAnalyzer",
          "autonomous supplier and product sourcing",
          platform::BusinessComponentSpec{
              "sourcing",
              "query",
              std::vector<std::string>{},
              false,
              false,
              4U * 1024U * 1024U}) {}

platform::OperationResult ReviewScoreAnalyzer::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::sourcing
