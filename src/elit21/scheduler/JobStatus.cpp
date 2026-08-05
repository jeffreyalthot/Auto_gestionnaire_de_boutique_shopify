#include "elit21/scheduler/JobStatus.h"

namespace elit21::scheduler {

JobStatus::JobStatus()
    : platform::BusinessComponent(
          "JobStatus",
          "scheduled and recurring job coordination",
          platform::BusinessComponentSpec{
              "scheduler",
              "model_operation",
              std::vector<std::string>{},
              false,
              false,
              4U * 1024U * 1024U}) {}

platform::OperationResult JobStatus::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::scheduler
