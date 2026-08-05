#include "elit21/security/KeyDerivation.h"

namespace elit21::security {

KeyDerivation::KeyDerivation()
    : platform::BusinessComponent(
          "KeyDerivation",
          "credential, signature and data protection",
          platform::BusinessComponentSpec{
              "security",
              "model_operation",
              std::vector<std::string>{},
              false,
              false,
              4U * 1024U * 1024U}) {}

platform::OperationResult KeyDerivation::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::security
