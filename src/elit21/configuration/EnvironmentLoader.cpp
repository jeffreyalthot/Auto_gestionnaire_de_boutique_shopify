#include "elit21/configuration/EnvironmentLoader.h"

namespace elit21::configuration {

EnvironmentLoader::EnvironmentLoader()
    : platform::BusinessComponent(
          "EnvironmentLoader",
          "validated layered configuration management",
          platform::BusinessComponentSpec{
              "configuration",
              "model_operation",
              std::vector<std::string>{},
              false,
              false,
              4U * 1024U * 1024U}) {}

platform::OperationResult EnvironmentLoader::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::configuration
