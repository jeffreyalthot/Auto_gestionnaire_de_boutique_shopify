#include "elit21/resilience/IdempotencyKey.h"

namespace elit21::resilience {

IdempotencyKey::IdempotencyKey()
    : platform::BusinessComponent(
          "IdempotencyKey",
          "retry, idempotency and failure recovery",
          platform::BusinessComponentSpec{
              "resilience",
              "model_operation",
              std::vector<std::string>{},
              false,
              false,
              4U * 1024U * 1024U}) {}

platform::OperationResult IdempotencyKey::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::resilience
