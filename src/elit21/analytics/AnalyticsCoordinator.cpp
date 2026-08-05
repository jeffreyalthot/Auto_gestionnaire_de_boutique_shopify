#include "elit21/analytics/AnalyticsCoordinator.h"

namespace elit21::analytics {

AnalyticsCoordinator::AnalyticsCoordinator()
    : platform::BusinessComponent(
          "AnalyticsCoordinator",
          "store performance analytics and forecasting",
          platform::BusinessComponentSpec{
              "analytics",
              "query",
              std::vector<std::string>{},
              false,
              false,
              4U * 1024U * 1024U}) {}

platform::OperationResult AnalyticsCoordinator::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::analytics
