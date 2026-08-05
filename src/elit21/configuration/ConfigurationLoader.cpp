#include "elit21/configuration/ConfigurationLoader.h"

namespace elit21::configuration {

ConfigurationLoader::ConfigurationLoader()
    : platform::BusinessComponent(
          "ConfigurationLoader",
          "validated layered configuration management",
          platform::BusinessComponentSpec{
              "configuration",
              "model_operation",
              std::vector<std::string>{},
              false,
              false,
              4U * 1024U * 1024U}) {}

platform::OperationResult ConfigurationLoader::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::configuration
