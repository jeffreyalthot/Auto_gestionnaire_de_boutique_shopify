#include "elit21/repositories/ConfigurationRepository.h"

namespace elit21::repositories {

ConfigurationRepository::ConfigurationRepository()
    : platform::BusinessComponent(
          "ConfigurationRepository",
          "persistent repository access",
          platform::BusinessComponentSpec{
              "repositories",
              "model_operation",
              std::vector<std::string>{},
              false,
              false,
              4U * 1024U * 1024U}) {}

platform::OperationResult ConfigurationRepository::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::repositories
