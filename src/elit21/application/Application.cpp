#include "elit21/application/Application.h"

namespace elit21::application {

Application::Application()
    : platform::BusinessComponent(
          "Application",
          "application lifecycle and dependency orchestration",
          platform::BusinessComponentSpec{
              "application",
              "model_operation",
              std::vector<std::string>{},
              false,
              false,
              4U * 1024U * 1024U}) {}

platform::OperationResult Application::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::application
