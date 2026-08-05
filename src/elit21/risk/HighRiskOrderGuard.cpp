#include "elit21/risk/HighRiskOrderGuard.h"

namespace elit21::risk {

HighRiskOrderGuard::HighRiskOrderGuard()
    : platform::BusinessComponent(
          "HighRiskOrderGuard",
          "fraud, loss and supplier risk controls",
          platform::BusinessComponentSpec{
              "risk",
              "validate_transform",
              std::vector<std::string>{"order_id"},
              false,
              false,
              4U * 1024U * 1024U}) {}

platform::OperationResult HighRiskOrderGuard::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::risk
