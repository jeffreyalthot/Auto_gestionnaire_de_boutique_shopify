#include "elit21/risk/RiskRule.h"

namespace elit21::risk {

RiskRule::RiskRule()
    : platform::BusinessComponent(
          "RiskRule",
          "fraud, loss and supplier risk controls",
          platform::BusinessComponentSpec{
              "risk",
              "model_operation",
              std::vector<std::string>{},
              false,
              false,
              4U * 1024U * 1024U}) {}

platform::OperationResult RiskRule::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::risk
