#include "elit21/core/ScopeGuard.h"

namespace elit21::core {

ScopeGuard::ScopeGuard()
    : platform::BusinessComponent(
          "ScopeGuard",
          "core domain primitive and invariant enforcement",
          platform::BusinessComponentSpec{
              "core",
              "validate_transform",
              std::vector<std::string>{},
              false,
              false,
              4U * 1024U * 1024U}) {}

platform::OperationResult ScopeGuard::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::core
