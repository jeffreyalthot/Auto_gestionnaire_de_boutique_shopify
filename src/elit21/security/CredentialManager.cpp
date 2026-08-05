#include "elit21/security/CredentialManager.h"

namespace elit21::security {

CredentialManager::CredentialManager()
    : platform::BusinessComponent(
          "CredentialManager",
          "credential, signature and data protection",
          platform::BusinessComponentSpec{
              "security",
              "orchestrate",
              std::vector<std::string>{},
              true,
              false,
              4U * 1024U * 1024U}) {}

platform::OperationResult CredentialManager::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::security
