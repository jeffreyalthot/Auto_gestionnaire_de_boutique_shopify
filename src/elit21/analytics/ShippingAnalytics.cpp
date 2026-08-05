#include "elit21/analytics/ShippingAnalytics.h"

namespace elit21::analytics {

ShippingAnalytics::ShippingAnalytics()
    : platform::BusinessComponent(
          "ShippingAnalytics",
          "store performance analytics and forecasting",
          platform::BusinessComponentSpec{
              "analytics",
              "query",
              std::vector<std::string>{},
              false,
              false,
              4U * 1024U * 1024U}) {}

platform::OperationResult ShippingAnalytics::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::analytics
