#include "elit21/application/ShutdownCoordinator.h"

namespace elit21::application {

ShutdownCoordinator::ShutdownCoordinator()
    : platform::BusinessComponent(
          "ShutdownCoordinator",
          "application lifecycle and dependency orchestration",
          platform::BusinessComponentSpec{
              "application",
              "orchestrate",
              std::vector<std::string>{},
              true,
              false,
              4U * 1024U * 1024U}) {}

platform::OperationResult ShutdownCoordinator::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::application
