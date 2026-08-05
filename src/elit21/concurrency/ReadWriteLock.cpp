#include "elit21/concurrency/ReadWriteLock.h"

namespace elit21::concurrency {

ReadWriteLock::ReadWriteLock()
    : platform::BusinessComponent(
          "ReadWriteLock",
          "bounded low-resource concurrency execution",
          platform::BusinessComponentSpec{
              "concurrency",
              "query",
              std::vector<std::string>{},
              false,
              false,
              4U * 1024U * 1024U}) {}

platform::OperationResult ReadWriteLock::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::concurrency
