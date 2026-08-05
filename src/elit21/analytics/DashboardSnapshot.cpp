#include "elit21/analytics/DashboardSnapshot.h"

namespace elit21::analytics {

DashboardSnapshot::DashboardSnapshot()
    : platform::BusinessComponent(
          "DashboardSnapshot",
          "store performance analytics and forecasting",
          platform::BusinessComponentSpec{
              "analytics",
              "query",
              std::vector<std::string>{},
              false,
              false,
              4U * 1024U * 1024U}) {}

platform::OperationResult DashboardSnapshot::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::analytics
