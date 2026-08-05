#include "elit21/resilience/RateLimiter.h"

namespace elit21::resilience {

RateLimiter::RateLimiter()
    : platform::BusinessComponent(
          "RateLimiter",
          "retry, idempotency and failure recovery",
          platform::BusinessComponentSpec{
              "resilience",
              "model_operation",
              std::vector<std::string>{},
              false,
              false,
              4U * 1024U * 1024U}) {}

platform::OperationResult RateLimiter::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::resilience
