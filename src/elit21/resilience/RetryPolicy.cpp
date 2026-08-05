#include "elit21/resilience/RetryPolicy.h"

namespace elit21::resilience {

RetryPolicy::RetryPolicy()
    : platform::BusinessComponent(
          "RetryPolicy",
          "retry, idempotency and failure recovery",
          platform::BusinessComponentSpec{
              "resilience",
              "validate_transform",
              std::vector<std::string>{},
              false,
              false,
              4U * 1024U * 1024U}) {}

platform::OperationResult RetryPolicy::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::resilience
