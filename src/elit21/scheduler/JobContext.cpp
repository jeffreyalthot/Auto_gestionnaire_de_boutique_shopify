#include "elit21/scheduler/JobContext.h"

namespace elit21::scheduler {

JobContext::JobContext()
    : platform::BusinessComponent(
          "JobContext",
          "scheduled and recurring job coordination",
          platform::BusinessComponentSpec{
              "scheduler",
              "model_operation",
              std::vector<std::string>{},
              false,
              false,
              4U * 1024U * 1024U}) {}

platform::OperationResult JobContext::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::scheduler
