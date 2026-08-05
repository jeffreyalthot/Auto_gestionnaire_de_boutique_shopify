#include "elit21/analytics/ConversionAnalytics.h"

namespace elit21::analytics {

ConversionAnalytics::ConversionAnalytics()
    : platform::BusinessComponent(
          "ConversionAnalytics",
          "store performance analytics and forecasting",
          platform::BusinessComponentSpec{
              "analytics",
              "query",
              std::vector<std::string>{},
              false,
              false,
              4U * 1024U * 1024U}) {}

platform::OperationResult ConversionAnalytics::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::analytics
