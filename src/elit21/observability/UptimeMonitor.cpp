#include "elit21/observability/UptimeMonitor.h"

namespace elit21::observability {

UptimeMonitor::UptimeMonitor()
    : platform::BusinessComponent(
          "UptimeMonitor",
          "health, metrics, traces, alerts and audit logs",
          platform::BusinessComponentSpec{
              "observability",
              "query",
              std::vector<std::string>{},
              false,
              false,
              4U * 1024U * 1024U}) {}

platform::OperationResult UptimeMonitor::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::observability
