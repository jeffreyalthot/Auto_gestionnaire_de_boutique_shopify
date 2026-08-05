#include "elit21/configuration/SecurityConfig.h"

namespace elit21::configuration {

SecurityConfig::SecurityConfig()
    : platform::BusinessComponent(
          "SecurityConfig",
          "validated layered configuration management",
          platform::BusinessComponentSpec{
              "configuration",
              "model_operation",
              std::vector<std::string>{},
              false,
              false,
              4U * 1024U * 1024U}) {}

platform::OperationResult SecurityConfig::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::configuration
