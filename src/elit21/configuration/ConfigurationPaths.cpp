#include "elit21/configuration/ConfigurationPaths.h"

namespace elit21::configuration {

ConfigurationPaths::ConfigurationPaths()
    : platform::BusinessComponent(
          "ConfigurationPaths",
          "validated layered configuration management",
          platform::BusinessComponentSpec{
              "configuration",
              "model_operation",
              std::vector<std::string>{},
              false,
              false,
              4U * 1024U * 1024U}) {}

platform::OperationResult ConfigurationPaths::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::configuration
