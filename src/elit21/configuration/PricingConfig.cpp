#include "elit21/configuration/PricingConfig.h"

namespace elit21::configuration {

PricingConfig::PricingConfig()
    : platform::BusinessComponent(
          "PricingConfig",
          "validated layered configuration management",
          platform::BusinessComponentSpec{
              "configuration",
              "model_operation",
              std::vector<std::string>{},
              false,
              false,
              4U * 1024U * 1024U}) {}

platform::OperationResult PricingConfig::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::configuration
