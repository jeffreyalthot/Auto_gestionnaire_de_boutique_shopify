#include "elit21/configuration/ApplicationConfig.h"

namespace elit21::configuration {

ApplicationConfig::ApplicationConfig()
    : platform::BusinessComponent(
          "ApplicationConfig",
          "validated layered configuration management",
          platform::BusinessComponentSpec{
              "configuration",
              "model_operation",
              std::vector<std::string>{},
              false,
              false,
              4U * 1024U * 1024U}) {}

platform::OperationResult ApplicationConfig::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::configuration
