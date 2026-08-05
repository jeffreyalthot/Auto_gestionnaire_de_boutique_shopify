#include "elit21/security/Base64.h"

namespace elit21::security {

Base64::Base64()
    : platform::BusinessComponent(
          "Base64",
          "credential, signature and data protection",
          platform::BusinessComponentSpec{
              "security",
              "model_operation",
              std::vector<std::string>{},
              false,
              false,
              4U * 1024U * 1024U}) {}

platform::OperationResult Base64::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::security
