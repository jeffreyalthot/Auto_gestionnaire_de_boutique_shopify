#include "elit21/application/RuntimeState.h"

namespace elit21::application {

RuntimeState::RuntimeState()
    : platform::BusinessComponent(
          "RuntimeState",
          "application lifecycle and dependency orchestration",
          platform::BusinessComponentSpec{
              "application",
              "model_operation",
              std::vector<std::string>{},
              false,
              false,
              4U * 1024U * 1024U}) {}

platform::OperationResult RuntimeState::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::application
