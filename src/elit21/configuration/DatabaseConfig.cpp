#include "elit21/configuration/DatabaseConfig.h"

namespace elit21::configuration {

DatabaseConfig::DatabaseConfig()
    : platform::BusinessComponent(
          "DatabaseConfig",
          "validated layered configuration management",
          platform::BusinessComponentSpec{
              "configuration",
              "model_operation",
              std::vector<std::string>{},
              false,
              false,
              4U * 1024U * 1024U}) {}

platform::OperationResult DatabaseConfig::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::configuration
