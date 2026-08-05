#include "elit21/scheduler/ScheduledJob.h"

namespace elit21::scheduler {

ScheduledJob::ScheduledJob()
    : platform::BusinessComponent(
          "ScheduledJob",
          "scheduled and recurring job coordination",
          platform::BusinessComponentSpec{
              "scheduler",
              "command",
              std::vector<std::string>{},
              true,
              false,
              4U * 1024U * 1024U}) {}

platform::OperationResult ScheduledJob::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::scheduler
