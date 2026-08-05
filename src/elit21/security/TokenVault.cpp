#include "elit21/security/TokenVault.h"

namespace elit21::security {

TokenVault::TokenVault()
    : platform::BusinessComponent(
          "TokenVault",
          "credential, signature and data protection",
          platform::BusinessComponentSpec{
              "security",
              "model_operation",
              std::vector<std::string>{},
              false,
              false,
              4U * 1024U * 1024U}) {}

platform::OperationResult TokenVault::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::security
