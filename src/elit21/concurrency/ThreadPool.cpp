#include "elit21/concurrency/ThreadPool.h"

namespace elit21::concurrency {

ThreadPool::ThreadPool()
    : platform::BusinessComponent(
          "ThreadPool",
          "bounded low-resource concurrency execution",
          platform::BusinessComponentSpec{
              "concurrency",
              "query",
              std::vector<std::string>{},
              false,
              false,
              4U * 1024U * 1024U}) {}

platform::OperationResult ThreadPool::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::concurrency
