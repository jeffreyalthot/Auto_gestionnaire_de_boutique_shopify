#include "elit21/configuration/LoggingConfig.h"

namespace elit21::configuration {

LoggingConfig::LoggingConfig()
    : platform::BusinessComponent(
          "LoggingConfig",
          "validated layered configuration management",
          platform::BusinessComponentSpec{
              "configuration",
              "model_operation",
              std::vector<std::string>{},
              false,
              false,
              4U * 1024U * 1024U}) {}

platform::OperationResult LoggingConfig::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::configuration
