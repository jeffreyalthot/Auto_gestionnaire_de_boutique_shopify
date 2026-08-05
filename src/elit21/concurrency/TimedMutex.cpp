#include "elit21/concurrency/TimedMutex.h"

namespace elit21::concurrency {

TimedMutex::TimedMutex()
    : platform::BusinessComponent(
          "TimedMutex",
          "bounded low-resource concurrency execution",
          platform::BusinessComponentSpec{
              "concurrency",
              "model_operation",
              std::vector<std::string>{},
              false,
              false,
              4U * 1024U * 1024U}) {}

platform::OperationResult TimedMutex::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::concurrency
