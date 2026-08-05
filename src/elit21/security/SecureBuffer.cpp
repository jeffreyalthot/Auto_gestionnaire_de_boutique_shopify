#include "elit21/security/SecureBuffer.h"

namespace elit21::security {

SecureBuffer::SecureBuffer()
    : platform::BusinessComponent(
          "SecureBuffer",
          "credential, signature and data protection",
          platform::BusinessComponentSpec{
              "security",
              "model_operation",
              std::vector<std::string>{},
              false,
              false,
              4U * 1024U * 1024U}) {}

platform::OperationResult SecureBuffer::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::security
