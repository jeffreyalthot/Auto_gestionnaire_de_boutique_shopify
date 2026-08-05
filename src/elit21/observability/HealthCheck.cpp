#include "elit21/observability/HealthCheck.h"

namespace elit21::observability {

HealthCheck::HealthCheck()
    : platform::BusinessComponent(
          "HealthCheck",
          "health, metrics, traces, alerts and audit logs",
          platform::BusinessComponentSpec{
              "observability",
              "query",
              std::vector<std::string>{},
              false,
              false,
              4U * 1024U * 1024U}) {}

platform::OperationResult HealthCheck::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::observability
