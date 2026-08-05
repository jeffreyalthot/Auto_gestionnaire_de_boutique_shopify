#include "elit21/security/Secret.h"

namespace elit21::security {

Secret::Secret()
    : platform::BusinessComponent(
          "Secret",
          "credential, signature and data protection",
          platform::BusinessComponentSpec{
              "security",
              "model_operation",
              std::vector<std::string>{},
              false,
              false,
              4U * 1024U * 1024U}) {}

platform::OperationResult Secret::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::security
