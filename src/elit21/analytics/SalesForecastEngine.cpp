#include "elit21/analytics/SalesForecastEngine.h"

namespace elit21::analytics {

SalesForecastEngine::SalesForecastEngine()
    : platform::BusinessComponent(
          "SalesForecastEngine",
          "store performance analytics and forecasting",
          platform::BusinessComponentSpec{
              "analytics",
              "orchestrate",
              std::vector<std::string>{},
              true,
              false,
              4U * 1024U * 1024U}) {}

platform::OperationResult SalesForecastEngine::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::analytics
