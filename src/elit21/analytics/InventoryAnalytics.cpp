#include "elit21/analytics/InventoryAnalytics.h"

namespace elit21::analytics {

InventoryAnalytics::InventoryAnalytics()
    : platform::BusinessComponent(
          "InventoryAnalytics",
          "store performance analytics and forecasting",
          platform::BusinessComponentSpec{
              "analytics",
              "query",
              std::vector<std::string>{},
              false,
              false,
              4U * 1024U * 1024U}) {}

platform::OperationResult InventoryAnalytics::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::analytics
