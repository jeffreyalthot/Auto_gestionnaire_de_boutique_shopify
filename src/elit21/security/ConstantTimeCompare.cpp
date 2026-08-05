#include "elit21/security/ConstantTimeCompare.h"

namespace elit21::security {

ConstantTimeCompare::ConstantTimeCompare()
    : platform::BusinessComponent(
          "ConstantTimeCompare",
          "credential, signature and data protection",
          platform::BusinessComponentSpec{
              "security",
              "model_operation",
              std::vector<std::string>{},
              false,
              false,
              4U * 1024U * 1024U}) {}

platform::OperationResult ConstantTimeCompare::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::security
