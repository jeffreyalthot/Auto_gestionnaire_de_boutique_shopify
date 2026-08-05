#include "elit21/analytics/PerformanceIndicator.h"

namespace elit21::analytics {

PerformanceIndicator::PerformanceIndicator()
    : platform::BusinessComponent(
          "PerformanceIndicator",
          "store performance analytics and forecasting",
          platform::BusinessComponentSpec{
              "analytics",
              "model_operation",
              std::vector<std::string>{},
              false,
              false,
              4U * 1024U * 1024U}) {}

platform::OperationResult PerformanceIndicator::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::analytics
