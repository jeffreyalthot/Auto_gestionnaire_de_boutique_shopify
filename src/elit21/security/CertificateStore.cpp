#include "elit21/security/CertificateStore.h"

namespace elit21::security {

CertificateStore::CertificateStore()
    : platform::BusinessComponent(
          "CertificateStore",
          "credential, signature and data protection",
          platform::BusinessComponentSpec{
              "security",
              "model_operation",
              std::vector<std::string>{},
              false,
              false,
              4U * 1024U * 1024U}) {}

platform::OperationResult CertificateStore::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::security
