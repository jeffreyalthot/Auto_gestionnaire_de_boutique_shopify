#include "elit21/application/ComponentRegistry.h"

namespace elit21::application {

ComponentRegistry::ComponentRegistry()
    : platform::BusinessComponent(
          "ComponentRegistry",
          "application lifecycle and dependency orchestration",
          platform::BusinessComponentSpec{
              "application",
              "model_operation",
              std::vector<std::string>{},
              false,
              false,
              4U * 1024U * 1024U}) {}

platform::OperationResult ComponentRegistry::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::application
