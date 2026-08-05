#include "elit21/analytics/RevenueAnalytics.h"

namespace elit21::analytics {

RevenueAnalytics::RevenueAnalytics()
    : platform::BusinessComponent(
          "RevenueAnalytics",
          "store performance analytics and forecasting",
          platform::BusinessComponentSpec{
              "analytics",
              "query",
              std::vector<std::string>{},
              false,
              false,
              4U * 1024U * 1024U}) {}

platform::OperationResult RevenueAnalytics::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::analytics
