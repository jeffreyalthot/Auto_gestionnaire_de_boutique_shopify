#include "elit21/application/ApplicationBootstrap.h"

namespace elit21::application {

ApplicationBootstrap::ApplicationBootstrap()
    : platform::BusinessComponent(
          "ApplicationBootstrap",
          "application lifecycle and dependency orchestration",
          platform::BusinessComponentSpec{
              "application",
              "model_operation",
              std::vector<std::string>{},
              false,
              false,
              4U * 1024U * 1024U}) {}

platform::OperationResult ApplicationBootstrap::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::application
