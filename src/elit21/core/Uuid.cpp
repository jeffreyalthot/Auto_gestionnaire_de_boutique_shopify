#include "elit21/core/Uuid.h"

namespace elit21::core {

Uuid::Uuid()
    : platform::BusinessComponent(
          "Uuid",
          "core domain primitive and invariant enforcement",
          platform::BusinessComponentSpec{
              "core",
              "model_operation",
              std::vector<std::string>{},
              false,
              false,
              4U * 1024U * 1024U}) {}

platform::OperationResult Uuid::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::core
