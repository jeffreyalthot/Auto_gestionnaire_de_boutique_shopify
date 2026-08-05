#include "elit21/configuration/ConfigurationValidator.h"

namespace elit21::configuration {

ConfigurationValidator::ConfigurationValidator()
    : platform::BusinessComponent(
          "ConfigurationValidator",
          "validated layered configuration management",
          platform::BusinessComponentSpec{
              "configuration",
              "validate_transform",
              std::vector<std::string>{},
              false,
              false,
              4U * 1024U * 1024U}) {}

platform::OperationResult ConfigurationValidator::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::configuration
