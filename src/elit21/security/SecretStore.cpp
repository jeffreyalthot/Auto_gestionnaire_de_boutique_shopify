#include "elit21/security/SecretStore.h"

namespace elit21::security {

SecretStore::SecretStore()
    : platform::BusinessComponent(
          "SecretStore",
          "credential, signature and data protection",
          platform::BusinessComponentSpec{
              "security",
              "model_operation",
              std::vector<std::string>{},
              false,
              false,
              4U * 1024U * 1024U}) {}

platform::OperationResult SecretStore::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::security
