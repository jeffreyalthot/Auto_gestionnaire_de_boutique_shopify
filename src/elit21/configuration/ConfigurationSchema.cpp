#include "elit21/configuration/ConfigurationSchema.h"

namespace elit21::configuration {

ConfigurationSchema::ConfigurationSchema()
    : platform::BusinessComponent(
          "ConfigurationSchema",
          "validated layered configuration management",
          platform::BusinessComponentSpec{
              "configuration",
              "model_operation",
              std::vector<std::string>{},
              false,
              false,
              4U * 1024U * 1024U}) {}

platform::OperationResult ConfigurationSchema::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::configuration
