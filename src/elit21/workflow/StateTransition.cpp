#include "elit21/workflow/StateTransition.h"

namespace elit21::workflow {

StateTransition::StateTransition()
    : platform::BusinessComponent(
          "StateTransition",
          "durable autonomous workflow and saga execution",
          platform::BusinessComponentSpec{
              "workflow",
              "model_operation",
              std::vector<std::string>{},
              false,
              false,
              4U * 1024U * 1024U}) {}

platform::OperationResult StateTransition::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::workflow
