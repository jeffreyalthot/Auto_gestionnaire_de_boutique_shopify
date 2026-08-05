#include "elit21/security/RandomGenerator.h"

namespace elit21::security {

RandomGenerator::RandomGenerator()
    : platform::BusinessComponent(
          "RandomGenerator",
          "credential, signature and data protection",
          platform::BusinessComponentSpec{
              "security",
              "model_operation",
              std::vector<std::string>{},
              false,
              false,
              4U * 1024U * 1024U}) {}

platform::OperationResult RandomGenerator::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::security
