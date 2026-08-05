#include "elit21/resilience/RetryExecutor.h"

namespace elit21::resilience {

RetryExecutor::RetryExecutor()
    : platform::BusinessComponent(
          "RetryExecutor",
          "retry, idempotency and failure recovery",
          platform::BusinessComponentSpec{
              "resilience",
              "model_operation",
              std::vector<std::string>{},
              false,
              false,
              4U * 1024U * 1024U}) {}

platform::OperationResult RetryExecutor::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::resilience
