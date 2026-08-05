#include "elit21/resilience/FailureClassifier.h"

namespace elit21::resilience {

FailureClassifier::FailureClassifier()
    : platform::BusinessComponent(
          "FailureClassifier",
          "retry, idempotency and failure recovery",
          platform::BusinessComponentSpec{
              "resilience",
              "validate_transform",
              std::vector<std::string>{},
              false,
              false,
              4U * 1024U * 1024U}) {}

platform::OperationResult FailureClassifier::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::resilience
