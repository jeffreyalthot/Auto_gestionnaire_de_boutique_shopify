#include "elit21/automation/TaskOrchestrator.h"

namespace elit21::automation {

TaskOrchestrator::TaskOrchestrator()
    : platform::BusinessComponent(
          "TaskOrchestrator",
          "autonomous policy and task orchestration",
          platform::BusinessComponentSpec{
              "automation",
              "orchestrate",
              std::vector<std::string>{},
              true,
              false,
              4U * 1024U * 1024U}) {}

platform::OperationResult TaskOrchestrator::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::automation
