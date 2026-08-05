#include "elit21/workflow/ActivityResult.h"

namespace elit21::workflow {

ActivityResult::ActivityResult()
    : platform::BusinessComponent(
          "ActivityResult",
          "durable autonomous workflow and saga execution",
          platform::BusinessComponentSpec{
              "workflow",
              "model_operation",
              std::vector<std::string>{},
              false,
              false,
              4U * 1024U * 1024U}) {}

platform::OperationResult ActivityResult::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::workflow
