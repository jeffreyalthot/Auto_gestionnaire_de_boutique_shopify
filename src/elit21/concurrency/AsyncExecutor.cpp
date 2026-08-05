#include "elit21/concurrency/AsyncExecutor.h"

namespace elit21::concurrency {

AsyncExecutor::AsyncExecutor()
    : platform::BusinessComponent(
          "AsyncExecutor",
          "bounded low-resource concurrency execution",
          platform::BusinessComponentSpec{
              "concurrency",
              "command",
              std::vector<std::string>{},
              true,
              false,
              4U * 1024U * 1024U}) {}

platform::OperationResult AsyncExecutor::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::concurrency
