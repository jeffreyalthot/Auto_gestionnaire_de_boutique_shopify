#include "elit21/observability/MetricRegistry.h"

namespace elit21::observability {

MetricRegistry::MetricRegistry()
    : platform::BusinessComponent(
          "MetricRegistry",
          "health, metrics, traces, alerts and audit logs",
          platform::BusinessComponentSpec{
              "observability",
              "model_operation",
              std::vector<std::string>{},
              false,
              false,
              4U * 1024U * 1024U}) {}

platform::OperationResult MetricRegistry::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::observability
