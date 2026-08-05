#include "elit21/resilience/Watchdog.h"

namespace elit21::resilience {

Watchdog::Watchdog()
    : platform::BusinessComponent(
          "Watchdog",
          "retry, idempotency and failure recovery",
          platform::BusinessComponentSpec{
              "resilience",
              "model_operation",
              std::vector<std::string>{},
              false,
              false,
              4U * 1024U * 1024U}) {}

platform::OperationResult Watchdog::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::resilience
