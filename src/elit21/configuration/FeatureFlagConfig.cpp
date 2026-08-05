#include "elit21/configuration/FeatureFlagConfig.h"

namespace elit21::configuration {

FeatureFlagConfig::FeatureFlagConfig()
    : platform::BusinessComponent(
          "FeatureFlagConfig",
          "validated layered configuration management",
          platform::BusinessComponentSpec{
              "configuration",
              "model_operation",
              std::vector<std::string>{},
              false,
              false,
              4U * 1024U * 1024U}) {}

platform::OperationResult FeatureFlagConfig::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::configuration
