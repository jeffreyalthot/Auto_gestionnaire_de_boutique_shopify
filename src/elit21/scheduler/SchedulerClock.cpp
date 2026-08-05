#include "elit21/scheduler/SchedulerClock.h"

namespace elit21::scheduler {

SchedulerClock::SchedulerClock()
    : platform::BusinessComponent(
          "SchedulerClock",
          "scheduled and recurring job coordination",
          platform::BusinessComponentSpec{
              "scheduler",
              "command",
              std::vector<std::string>{},
              true,
              false,
              4U * 1024U * 1024U}) {}

platform::OperationResult SchedulerClock::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::scheduler
