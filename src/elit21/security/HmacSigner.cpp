#include "elit21/security/HmacSigner.h"

namespace elit21::security {

HmacSigner::HmacSigner()
    : platform::BusinessComponent(
          "HmacSigner",
          "credential, signature and data protection",
          platform::BusinessComponentSpec{
              "security",
              "model_operation",
              std::vector<std::string>{},
              false,
              false,
              4U * 1024U * 1024U}) {}

platform::OperationResult HmacSigner::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::security
