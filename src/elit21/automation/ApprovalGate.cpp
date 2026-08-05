#include "elit21/automation/ApprovalGate.h"

namespace elit21::automation {

ApprovalGate::ApprovalGate()
    : platform::BusinessComponent(
          "ApprovalGate",
          "autonomous policy and task orchestration",
          platform::BusinessComponentSpec{
              "automation",
              "model_operation",
              std::vector<std::string>{},
              false,
              false,
              4U * 1024U * 1024U}) {}

platform::OperationResult ApprovalGate::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::automation
