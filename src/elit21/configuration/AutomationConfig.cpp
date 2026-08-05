#include "elit21/configuration/AutomationConfig.h"

namespace elit21::configuration {

AutomationConfig::AutomationConfig()
    : platform::BusinessComponent(
          "AutomationConfig",
          "validated layered configuration management",
          platform::BusinessComponentSpec{
              "configuration",
              "model_operation",
              std::vector<std::string>{},
              false,
              false,
              4U * 1024U * 1024U}) {}

platform::OperationResult AutomationConfig::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::configuration
