#include "elit21/resilience/IdempotencyGuard.h"

namespace elit21::resilience {

IdempotencyGuard::IdempotencyGuard()
    : platform::BusinessComponent(
          "IdempotencyGuard",
          "retry, idempotency and failure recovery",
          platform::BusinessComponentSpec{
              "resilience",
              "validate_transform",
              std::vector<std::string>{},
              false,
              false,
              4U * 1024U * 1024U}) {}

platform::OperationResult IdempotencyGuard::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::resilience
