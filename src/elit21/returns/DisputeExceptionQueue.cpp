#include "elit21/returns/DisputeExceptionQueue.h"

namespace elit21::returns {

DisputeExceptionQueue::DisputeExceptionQueue()
    : platform::BusinessComponent(
          "DisputeExceptionQueue",
          "return, refund and dispute coordination",
          platform::BusinessComponentSpec{
              "returns",
              "model_operation",
              std::vector<std::string>{},
              false,
              false,
              4U * 1024U * 1024U}) {}

platform::OperationResult DisputeExceptionQueue::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::returns
