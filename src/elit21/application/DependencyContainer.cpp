#include "elit21/application/DependencyContainer.h"

namespace elit21::application {

DependencyContainer::DependencyContainer()
    : platform::BusinessComponent(
          "DependencyContainer",
          "application lifecycle and dependency orchestration",
          platform::BusinessComponentSpec{
              "application",
              "model_operation",
              std::vector<std::string>{},
              false,
              false,
              4U * 1024U * 1024U}) {}

platform::OperationResult DependencyContainer::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::application
