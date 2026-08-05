#include "elit21/application/ServiceRegistry.h"

namespace elit21::application {

ServiceRegistry::ServiceRegistry()
    : platform::BusinessComponent(
          "ServiceRegistry",
          "application lifecycle and dependency orchestration",
          platform::BusinessComponentSpec{
              "application",
              "orchestrate",
              std::vector<std::string>{},
              true,
              false,
              4U * 1024U * 1024U}) {}

platform::OperationResult ServiceRegistry::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::application
