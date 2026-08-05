#include "elit21/analytics/ProfitabilityAnalytics.h"

namespace elit21::analytics {

ProfitabilityAnalytics::ProfitabilityAnalytics()
    : platform::BusinessComponent(
          "ProfitabilityAnalytics",
          "store performance analytics and forecasting",
          platform::BusinessComponentSpec{
              "analytics",
              "query",
              std::vector<std::string>{},
              false,
              false,
              4U * 1024U * 1024U}) {}

platform::OperationResult ProfitabilityAnalytics::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::analytics
