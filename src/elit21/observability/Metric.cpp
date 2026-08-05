#include "elit21/observability/Metric.h"

namespace elit21::observability {

Metric::Metric()
    : platform::BusinessComponent(
          "Metric",
          "health, metrics, traces, alerts and audit logs",
          platform::BusinessComponentSpec{
              "observability",
              "model_operation",
              std::vector<std::string>{},
              false,
              false,
              4U * 1024U * 1024U}) {}

platform::OperationResult Metric::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::observability
