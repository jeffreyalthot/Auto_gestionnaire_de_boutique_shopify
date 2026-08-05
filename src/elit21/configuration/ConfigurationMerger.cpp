#include "elit21/configuration/ConfigurationMerger.h"

namespace elit21::configuration {

ConfigurationMerger::ConfigurationMerger()
    : platform::BusinessComponent(
          "ConfigurationMerger",
          "validated layered configuration management",
          platform::BusinessComponentSpec{
              "configuration",
              "model_operation",
              std::vector<std::string>{},
              false,
              false,
              4U * 1024U * 1024U}) {}

platform::OperationResult ConfigurationMerger::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::configuration
