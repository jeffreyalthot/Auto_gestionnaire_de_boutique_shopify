#include "elit21/analytics/ProductPerformanceAnalyzer.h"

namespace elit21::analytics {

ProductPerformanceAnalyzer::ProductPerformanceAnalyzer()
    : platform::BusinessComponent(
          "ProductPerformanceAnalyzer",
          "store performance analytics and forecasting",
          platform::BusinessComponentSpec{
              "analytics",
              "query",
              std::vector<std::string>{},
              false,
              false,
              4U * 1024U * 1024U}) {}

platform::OperationResult ProductPerformanceAnalyzer::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::analytics
