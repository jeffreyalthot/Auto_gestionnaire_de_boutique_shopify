#include "elit21/concurrency/ProcessLock.h"

namespace elit21::concurrency {

ProcessLock::ProcessLock()
    : platform::BusinessComponent(
          "ProcessLock",
          "bounded low-resource concurrency execution",
          platform::BusinessComponentSpec{
              "concurrency",
              "command",
              std::vector<std::string>{},
              true,
              false,
              4U * 1024U * 1024U}) {}

platform::OperationResult ProcessLock::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::concurrency
