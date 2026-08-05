#include "elit21/application/ApplicationLifecycle.h"

namespace elit21::application {

ApplicationLifecycle::ApplicationLifecycle()
    : platform::BusinessComponent(
          "ApplicationLifecycle",
          "application lifecycle and dependency orchestration",
          platform::BusinessComponentSpec{
              "application",
              "model_operation",
              std::vector<std::string>{},
              false,
              false,
              4U * 1024U * 1024U}) {}

platform::OperationResult ApplicationLifecycle::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::application
