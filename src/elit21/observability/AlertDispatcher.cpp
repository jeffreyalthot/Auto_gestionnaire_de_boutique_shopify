#include "elit21/observability/AlertDispatcher.h"

namespace elit21::observability {

AlertDispatcher::AlertDispatcher()
    : platform::BusinessComponent(
          "AlertDispatcher",
          "health, metrics, traces, alerts and audit logs",
          platform::BusinessComponentSpec{
              "observability",
              "command",
              std::vector<std::string>{},
              true,
              false,
              4U * 1024U * 1024U}) {}

platform::OperationResult AlertDispatcher::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::observability
