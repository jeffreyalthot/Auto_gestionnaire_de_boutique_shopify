#include "elit21/concurrency/ConcurrentQueue.h"

namespace elit21::concurrency {

ConcurrentQueue::ConcurrentQueue()
    : platform::BusinessComponent(
          "ConcurrentQueue",
          "bounded low-resource concurrency execution",
          platform::BusinessComponentSpec{
              "concurrency",
              "model_operation",
              std::vector<std::string>{},
              false,
              false,
              4U * 1024U * 1024U}) {}

platform::OperationResult ConcurrentQueue::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::concurrency
