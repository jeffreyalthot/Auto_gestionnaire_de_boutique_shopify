#include "elit21/security/KeyRotationManager.h"

namespace elit21::security {

KeyRotationManager::KeyRotationManager()
    : platform::BusinessComponent(
          "KeyRotationManager",
          "credential, signature and data protection",
          platform::BusinessComponentSpec{
              "security",
              "orchestrate",
              std::vector<std::string>{},
              true,
              false,
              4U * 1024U * 1024U}) {}

platform::OperationResult KeyRotationManager::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::security
