#include "elit21/security/ApiCredential.h"

namespace elit21::security {

ApiCredential::ApiCredential()
    : platform::BusinessComponent(
          "ApiCredential",
          "credential, signature and data protection",
          platform::BusinessComponentSpec{
              "security",
              "model_operation",
              std::vector<std::string>{},
              false,
              false,
              4U * 1024U * 1024U}) {}

platform::OperationResult ApiCredential::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::security
