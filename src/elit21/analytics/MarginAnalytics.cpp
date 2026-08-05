#include "elit21/analytics/MarginAnalytics.h"

namespace elit21::analytics {

MarginAnalytics::MarginAnalytics()
    : platform::BusinessComponent(
          "MarginAnalytics",
          "store performance analytics and forecasting",
          platform::BusinessComponentSpec{
              "analytics",
              "query",
              std::vector<std::string>{},
              false,
              false,
              4U * 1024U * 1024U}) {}

platform::OperationResult MarginAnalytics::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::analytics
