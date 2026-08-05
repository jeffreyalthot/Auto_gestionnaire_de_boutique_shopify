#include "elit21/core/Optional.h"

namespace elit21::core {

Optional::Optional()
    : platform::BusinessComponent(
          "Optional",
          "core domain primitive and invariant enforcement",
          platform::BusinessComponentSpec{
              "core",
              "model_operation",
              std::vector<std::string>{},
              false,
              false,
              4U * 1024U * 1024U}) {}

platform::OperationResult Optional::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::core
