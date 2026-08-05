#include "elit21/scheduler/RecurringJob.h"

namespace elit21::scheduler {

RecurringJob::RecurringJob()
    : platform::BusinessComponent(
          "RecurringJob",
          "scheduled and recurring job coordination",
          platform::BusinessComponentSpec{
              "scheduler",
              "model_operation",
              std::vector<std::string>{},
              false,
              false,
              4U * 1024U * 1024U}) {}

platform::OperationResult RecurringJob::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::scheduler
