#include "elit21/automation/ExceptionEscalationService.h"

namespace elit21::automation {

ExceptionEscalationService::ExceptionEscalationService()
    : platform::BusinessComponent(
          "ExceptionEscalationService",
          "autonomous policy and task orchestration",
          platform::BusinessComponentSpec{
              "automation",
              "orchestrate",
              std::vector<std::string>{},
              true,
              false,
              4U * 1024U * 1024U}) {}

platform::OperationResult ExceptionEscalationService::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::automation
