#include "elit21/concurrency/CancellationToken.h"

namespace elit21::concurrency {

CancellationToken::CancellationToken()
    : platform::BusinessComponent(
          "CancellationToken",
          "bounded low-resource concurrency execution",
          platform::BusinessComponentSpec{
              "concurrency",
              "command",
              std::vector<std::string>{},
              true,
              false,
              4U * 1024U * 1024U}) {}

platform::OperationResult CancellationToken::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::concurrency
