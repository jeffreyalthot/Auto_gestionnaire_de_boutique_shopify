#include "elit21/observability/AuditLogger.h"

namespace elit21::observability {

AuditLogger::AuditLogger()
    : platform::BusinessComponent(
          "AuditLogger",
          "health, metrics, traces, alerts and audit logs",
          platform::BusinessComponentSpec{
              "observability",
              "model_operation",
              std::vector<std::string>{},
              false,
              false,
              4U * 1024U * 1024U}) {}

platform::OperationResult AuditLogger::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::observability
