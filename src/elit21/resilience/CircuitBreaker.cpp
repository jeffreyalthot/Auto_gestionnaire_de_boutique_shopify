#include "elit21/resilience/CircuitBreaker.h"

namespace elit21::resilience {

CircuitBreaker::CircuitBreaker()
    : platform::BusinessComponent(
          "CircuitBreaker",
          "retry, idempotency and failure recovery",
          platform::BusinessComponentSpec{
              "resilience",
              "model_operation",
              std::vector<std::string>{},
              false,
              false,
              4U * 1024U * 1024U}) {}

platform::OperationResult CircuitBreaker::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::resilience
