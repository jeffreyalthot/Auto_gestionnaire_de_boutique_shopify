#include "elit21/resilience/BackoffPolicy.h"

namespace elit21::resilience {

BackoffPolicy::BackoffPolicy()
    : platform::BusinessComponent(
          "BackoffPolicy",
          "retry, idempotency and failure recovery",
          platform::BusinessComponentSpec{
              "resilience",
              "validate_transform",
              std::vector<std::string>{},
              false,
              false,
              4U * 1024U * 1024U}) {}

platform::OperationResult BackoffPolicy::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::resilience
