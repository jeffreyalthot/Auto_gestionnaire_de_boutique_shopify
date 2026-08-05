#include "elit21/observability/HealthMonitor.h"

namespace elit21::observability {

HealthMonitor::HealthMonitor()
    : platform::BusinessComponent(
          "HealthMonitor",
          "health, metrics, traces, alerts and audit logs",
          platform::BusinessComponentSpec{
              "observability",
              "query",
              std::vector<std::string>{},
              false,
              false,
              4U * 1024U * 1024U}) {}

platform::OperationResult HealthMonitor::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::observability
