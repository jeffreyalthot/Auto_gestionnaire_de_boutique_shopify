#include "elit21/repositories/AuditRepository.h"

namespace elit21::repositories {

AuditRepository::AuditRepository()
    : platform::BusinessComponent(
          "AuditRepository",
          "persistent repository access",
          platform::BusinessComponentSpec{
              "repositories",
              "model_operation",
              std::vector<std::string>{},
              false,
              false,
              4U * 1024U * 1024U}) {}

platform::OperationResult AuditRepository::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::repositories
