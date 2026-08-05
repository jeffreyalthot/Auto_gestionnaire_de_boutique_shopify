#include "elit21/security/AuditChain.h"

namespace elit21::security {

AuditChain::AuditChain()
    : platform::BusinessComponent(
          "AuditChain",
          "credential, signature and data protection",
          platform::BusinessComponentSpec{
              "security",
              "model_operation",
              std::vector<std::string>{},
              false,
              false,
              4U * 1024U * 1024U}) {}

platform::OperationResult AuditChain::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::security
