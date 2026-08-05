#include "elit21/configuration/InventoryConfig.h"

namespace elit21::configuration {

InventoryConfig::InventoryConfig()
    : platform::BusinessComponent(
          "InventoryConfig",
          "validated layered configuration management",
          platform::BusinessComponentSpec{
              "configuration",
              "model_operation",
              std::vector<std::string>{},
              false,
              false,
              4U * 1024U * 1024U}) {}

platform::OperationResult InventoryConfig::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::configuration
