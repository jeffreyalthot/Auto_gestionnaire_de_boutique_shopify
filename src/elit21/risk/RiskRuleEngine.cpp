#include "elit21/risk/RiskRuleEngine.h"

namespace elit21::risk {

RiskRuleEngine::RiskRuleEngine()
    : platform::BusinessComponent(
          "RiskRuleEngine",
          "fraud, loss and supplier risk controls",
          platform::BusinessComponentSpec{
              "risk",
              "orchestrate",
              std::vector<std::string>{},
              true,
              false,
              4U * 1024U * 1024U}) {}

platform::OperationResult RiskRuleEngine::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::risk
