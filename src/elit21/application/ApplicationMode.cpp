#include "elit21/application/ApplicationMode.h"

namespace elit21::application {

ApplicationMode::ApplicationMode()
    : platform::BusinessComponent(
          "ApplicationMode",
          "application lifecycle and dependency orchestration",
          platform::BusinessComponentSpec{
              "application",
              "model_operation",
              std::vector<std::string>{},
              false,
              false,
              4U * 1024U * 1024U}) {}

platform::OperationResult ApplicationMode::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::application
