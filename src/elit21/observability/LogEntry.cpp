#include "elit21/observability/LogEntry.h"

namespace elit21::observability {

LogEntry::LogEntry()
    : platform::BusinessComponent(
          "LogEntry",
          "health, metrics, traces, alerts and audit logs",
          platform::BusinessComponentSpec{
              "observability",
              "model_operation",
              std::vector<std::string>{},
              false,
              false,
              4U * 1024U * 1024U}) {}

platform::OperationResult LogEntry::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::observability
