#include "elit21/resilience/DeadLetterQueue.h"

namespace elit21::resilience {

DeadLetterQueue::DeadLetterQueue()
    : platform::BusinessComponent(
          "DeadLetterQueue",
          "retry, idempotency and failure recovery",
          platform::BusinessComponentSpec{
              "resilience",
              "model_operation",
              std::vector<std::string>{},
              false,
              false,
              4U * 1024U * 1024U}) {}

platform::OperationResult DeadLetterQueue::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::resilience
