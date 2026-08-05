#include "elit21/analytics/OrderAnalytics.h"

namespace elit21::analytics {

OrderAnalytics::OrderAnalytics()
    : platform::BusinessComponent(
          "OrderAnalytics",
          "store performance analytics and forecasting",
          platform::BusinessComponentSpec{
              "analytics",
              "query",
              std::vector<std::string>{},
              false,
              false,
              4U * 1024U * 1024U}) {}

platform::OperationResult OrderAnalytics::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::analytics
