#include "elit21/security/SignatureVerifier.h"

namespace elit21::security {

SignatureVerifier::SignatureVerifier()
    : platform::BusinessComponent(
          "SignatureVerifier",
          "credential, signature and data protection",
          platform::BusinessComponentSpec{
              "security",
              "model_operation",
              std::vector<std::string>{},
              false,
              false,
              4U * 1024U * 1024U}) {}

platform::OperationResult SignatureVerifier::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::security
