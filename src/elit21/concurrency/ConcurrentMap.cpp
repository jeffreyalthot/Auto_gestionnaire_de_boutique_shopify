#include "elit21/concurrency/ConcurrentMap.h"

namespace elit21::concurrency {

ConcurrentMap::ConcurrentMap()
    : platform::BusinessComponent(
          "ConcurrentMap",
          "bounded low-resource concurrency execution",
          platform::BusinessComponentSpec{
              "concurrency",
              "model_operation",
              std::vector<std::string>{},
              false,
              false,
              4U * 1024U * 1024U}) {}

platform::OperationResult ConcurrentMap::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::concurrency
