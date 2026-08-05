#include "elit21/compliance/RegulatoryExceptionQueue.h"

namespace elit21::compliance {

RegulatoryExceptionQueue::RegulatoryExceptionQueue()
    : platform::BusinessComponent(
          "RegulatoryExceptionQueue",
          "Canadian product, privacy and address compliance",
          platform::BusinessComponentSpec{
              "compliance",
              "model_operation",
              std::vector<std::string>{},
              false,
              false,
              4U * 1024U * 1024U}) {}

platform::OperationResult RegulatoryExceptionQueue::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::compliance
