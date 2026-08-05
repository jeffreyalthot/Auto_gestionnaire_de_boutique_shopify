#include "elit21/automation/DryRunExecutor.h"

namespace elit21::automation {

DryRunExecutor::DryRunExecutor()
    : platform::BusinessComponent(
          "DryRunExecutor",
          "autonomous policy and task orchestration",
          platform::BusinessComponentSpec{
              "automation",
              "model_operation",
              std::vector<std::string>{},
              false,
              false,
              4U * 1024U * 1024U}) {}

platform::OperationResult DryRunExecutor::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::automation
