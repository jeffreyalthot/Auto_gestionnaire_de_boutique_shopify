#include "elit21/application/RuntimeCoordinator.h"

namespace elit21::application {

RuntimeCoordinator::RuntimeCoordinator()
    : platform::BusinessComponent(
          "RuntimeCoordinator",
          "application lifecycle and dependency orchestration",
          platform::BusinessComponentSpec{
              "application",
              "orchestrate",
              std::vector<std::string>{},
              true,
              false,
              4U * 1024U * 1024U}) {}

platform::OperationResult RuntimeCoordinator::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::application
