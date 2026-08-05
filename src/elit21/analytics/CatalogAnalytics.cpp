#include "elit21/analytics/CatalogAnalytics.h"

namespace elit21::analytics {

CatalogAnalytics::CatalogAnalytics()
    : platform::BusinessComponent(
          "CatalogAnalytics",
          "store performance analytics and forecasting",
          platform::BusinessComponentSpec{
              "analytics",
              "query",
              std::vector<std::string>{},
              false,
              false,
              4U * 1024U * 1024U}) {}

platform::OperationResult CatalogAnalytics::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::analytics
