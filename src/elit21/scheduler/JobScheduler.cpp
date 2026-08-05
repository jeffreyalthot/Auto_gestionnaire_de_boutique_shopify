#include "elit21/scheduler/JobScheduler.h"

namespace elit21::scheduler {

JobScheduler::JobScheduler()
    : platform::BusinessComponent(
          "JobScheduler",
          "scheduled and recurring job coordination",
          platform::BusinessComponentSpec{
              "scheduler",
              "command",
              std::vector<std::string>{},
              true,
              false,
              4U * 1024U * 1024U}) {}

platform::OperationResult JobScheduler::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::scheduler
