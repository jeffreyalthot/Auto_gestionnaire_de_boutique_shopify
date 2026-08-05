#include "elit21/workflow/Saga.h"

namespace elit21::workflow {

Saga::Saga()
    : platform::BusinessComponent(
          "Saga",
          "durable autonomous workflow and saga execution",
          platform::BusinessComponentSpec{
              "workflow",
              "orchestrate",
              std::vector<std::string>{},
              true,
              false,
              4U * 1024U * 1024U}) {}

platform::OperationResult Saga::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::workflow
