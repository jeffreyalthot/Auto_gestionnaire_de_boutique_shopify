#include "elit21/concurrency/FutureUtils.h"

namespace elit21::concurrency {

FutureUtils::FutureUtils()
    : platform::BusinessComponent(
          "FutureUtils",
          "bounded low-resource concurrency execution",
          platform::BusinessComponentSpec{
              "concurrency",
              "model_operation",
              std::vector<std::string>{},
              false,
              false,
              4U * 1024U * 1024U}) {}

platform::OperationResult FutureUtils::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::concurrency
