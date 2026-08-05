#include "elit21/configuration/ConfigurationWatcher.h"

namespace elit21::configuration {

ConfigurationWatcher::ConfigurationWatcher()
    : platform::BusinessComponent(
          "ConfigurationWatcher",
          "validated layered configuration management",
          platform::BusinessComponentSpec{
              "configuration",
              "model_operation",
              std::vector<std::string>{},
              false,
              false,
              4U * 1024U * 1024U}) {}

platform::OperationResult ConfigurationWatcher::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::configuration
