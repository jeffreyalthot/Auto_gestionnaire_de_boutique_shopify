#include "elit21/resilience/RecoveryManager.h"

namespace elit21::resilience {

RecoveryManager::RecoveryManager()
    : platform::BusinessComponent(
          "RecoveryManager",
          "retry, idempotency and failure recovery",
          platform::BusinessComponentSpec{
              "resilience",
              "orchestrate",
              std::vector<std::string>{},
              true,
              false,
              4U * 1024U * 1024U}) {}

platform::OperationResult RecoveryManager::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::resilience
