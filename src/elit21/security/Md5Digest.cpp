#include "elit21/security/Md5Digest.h"

namespace elit21::security {

Md5Digest::Md5Digest()
    : platform::BusinessComponent(
          "Md5Digest",
          "credential, signature and data protection",
          platform::BusinessComponentSpec{
              "security",
              "model_operation",
              std::vector<std::string>{},
              false,
              false,
              4U * 1024U * 1024U}) {}

platform::OperationResult Md5Digest::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::security
