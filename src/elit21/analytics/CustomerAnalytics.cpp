#include "elit21/analytics/CustomerAnalytics.h"

namespace elit21::analytics {

CustomerAnalytics::CustomerAnalytics()
    : platform::BusinessComponent(
          "CustomerAnalytics",
          "store performance analytics and forecasting",
          platform::BusinessComponentSpec{
              "analytics",
              "query",
              std::vector<std::string>{},
              false,
              false,
              4U * 1024U * 1024U}) {}

platform::OperationResult CustomerAnalytics::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::analytics
