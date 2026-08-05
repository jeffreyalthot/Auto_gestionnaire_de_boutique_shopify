#include "elit21/concurrency/Semaphore.h"

namespace elit21::concurrency {

Semaphore::Semaphore()
    : platform::BusinessComponent(
          "Semaphore",
          "bounded low-resource concurrency execution",
          platform::BusinessComponentSpec{
              "concurrency",
              "model_operation",
              std::vector<std::string>{},
              false,
              false,
              4U * 1024U * 1024U}) {}

platform::OperationResult Semaphore::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::concurrency
