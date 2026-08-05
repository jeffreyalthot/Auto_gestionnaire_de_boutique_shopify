#include "elit21/concurrency/WorkerThread.h"

namespace elit21::concurrency {

WorkerThread::WorkerThread()
    : platform::BusinessComponent(
          "WorkerThread",
          "bounded low-resource concurrency execution",
          platform::BusinessComponentSpec{
              "concurrency",
              "query",
              std::vector<std::string>{},
              false,
              false,
              4U * 1024U * 1024U}) {}

platform::OperationResult WorkerThread::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::concurrency
