#include "elit21/resilience/FallbackPolicy.h"

namespace elit21::resilience {

FallbackPolicy::FallbackPolicy()
    : platform::BusinessComponent(
          "FallbackPolicy",
          "retry, idempotency and failure recovery",
          platform::BusinessComponentSpec{
              "resilience",
              "validate_transform",
              std::vector<std::string>{},
              false,
              false,
              4U * 1024U * 1024U}) {}

platform::OperationResult FallbackPolicy::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::resilience
