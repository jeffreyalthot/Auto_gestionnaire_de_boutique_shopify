#include "elit21/observability/HealthCheckRegistry.h"

namespace elit21::observability {

HealthCheckRegistry::HealthCheckRegistry()
    : platform::BusinessComponent(
          "HealthCheckRegistry",
          "health, metrics, traces, alerts and audit logs",
          platform::BusinessComponentSpec{
              "observability",
              "query",
              std::vector<std::string>{},
              false,
              false,
              4U * 1024U * 1024U}) {}

platform::OperationResult HealthCheckRegistry::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::observability
