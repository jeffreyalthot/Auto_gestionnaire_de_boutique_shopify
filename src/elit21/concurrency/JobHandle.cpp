#include "elit21/concurrency/JobHandle.h"

namespace elit21::concurrency {

JobHandle::JobHandle()
    : platform::BusinessComponent(
          "JobHandle",
          "bounded low-resource concurrency execution",
          platform::BusinessComponentSpec{
              "concurrency",
              "model_operation",
              std::vector<std::string>{},
              false,
              false,
              4U * 1024U * 1024U}) {}

platform::OperationResult JobHandle::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::concurrency
