#include "elit21/analytics/FulfillmentAnalytics.h"

namespace elit21::analytics {

FulfillmentAnalytics::FulfillmentAnalytics()
    : platform::BusinessComponent(
          "FulfillmentAnalytics",
          "store performance analytics and forecasting",
          platform::BusinessComponentSpec{
              "analytics",
              "query",
              std::vector<std::string>{},
              false,
              false,
              4U * 1024U * 1024U}) {}

platform::OperationResult FulfillmentAnalytics::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::analytics
