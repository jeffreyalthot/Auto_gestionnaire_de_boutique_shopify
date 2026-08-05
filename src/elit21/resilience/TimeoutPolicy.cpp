#include "elit21/resilience/TimeoutPolicy.h"

namespace elit21::resilience {

TimeoutPolicy::TimeoutPolicy()
    : platform::BusinessComponent(
          "TimeoutPolicy",
          "retry, idempotency and failure recovery",
          platform::BusinessComponentSpec{
              "resilience",
              "validate_transform",
              std::vector<std::string>{},
              false,
              false,
              4U * 1024U * 1024U}) {}

platform::OperationResult TimeoutPolicy::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::resilience
