#include "elit21/concurrency/ThreadSafeValue.h"

namespace elit21::concurrency {

ThreadSafeValue::ThreadSafeValue()
    : platform::BusinessComponent(
          "ThreadSafeValue",
          "bounded low-resource concurrency execution",
          platform::BusinessComponentSpec{
              "concurrency",
              "query",
              std::vector<std::string>{},
              false,
              false,
              4U * 1024U * 1024U}) {}

platform::OperationResult ThreadSafeValue::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::concurrency
