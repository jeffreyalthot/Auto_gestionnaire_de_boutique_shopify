#include "elit21/configuration/TerminalConfig.h"

namespace elit21::configuration {

TerminalConfig::TerminalConfig()
    : platform::BusinessComponent(
          "TerminalConfig",
          "validated layered configuration management",
          platform::BusinessComponentSpec{
              "configuration",
              "model_operation",
              std::vector<std::string>{},
              false,
              false,
              4U * 1024U * 1024U}) {}

platform::OperationResult TerminalConfig::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::configuration
