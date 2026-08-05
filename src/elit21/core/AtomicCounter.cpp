#include "elit21/core/AtomicCounter.h"

namespace elit21::core {

AtomicCounter::AtomicCounter()
    : platform::BusinessComponent(
          "AtomicCounter",
          "core domain primitive and invariant enforcement",
          platform::BusinessComponentSpec{
              "core",
              "model_operation",
              std::vector<std::string>{},
              false,
              false,
              4U * 1024U * 1024U}) {}

platform::OperationResult AtomicCounter::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::core
