#include "elit21/scheduler/JobRegistry.h"

namespace elit21::scheduler {

JobRegistry::JobRegistry()
    : platform::BusinessComponent(
          "JobRegistry",
          "scheduled and recurring job coordination",
          platform::BusinessComponentSpec{
              "scheduler",
              "model_operation",
              std::vector<std::string>{},
              false,
              false,
              4U * 1024U * 1024U}) {}

platform::OperationResult JobRegistry::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::scheduler
