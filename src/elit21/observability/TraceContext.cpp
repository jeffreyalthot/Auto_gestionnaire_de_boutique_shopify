#include "elit21/observability/TraceContext.h"

namespace elit21::observability {

TraceContext::TraceContext()
    : platform::BusinessComponent(
          "TraceContext",
          "health, metrics, traces, alerts and audit logs",
          platform::BusinessComponentSpec{
              "observability",
              "model_operation",
              std::vector<std::string>{},
              false,
              false,
              4U * 1024U * 1024U}) {}

platform::OperationResult TraceContext::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::observability
