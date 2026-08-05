#include "elit21/resilience/Bulkhead.h"

namespace elit21::resilience {

Bulkhead::Bulkhead()
    : platform::BusinessComponent(
          "Bulkhead",
          "retry, idempotency and failure recovery",
          platform::BusinessComponentSpec{
              "resilience",
              "model_operation",
              std::vector<std::string>{},
              false,
              false,
              4U * 1024U * 1024U}) {}

platform::OperationResult Bulkhead::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::resilience
