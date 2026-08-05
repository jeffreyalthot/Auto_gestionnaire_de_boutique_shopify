#include "elit21/risk/FraudRiskEngine.h"

namespace elit21::risk {

FraudRiskEngine::FraudRiskEngine()
    : platform::BusinessComponent(
          "FraudRiskEngine",
          "fraud, loss and supplier risk controls",
          platform::BusinessComponentSpec{
              "risk",
              "orchestrate",
              std::vector<std::string>{},
              true,
              false,
              4U * 1024U * 1024U}) {}

platform::OperationResult FraudRiskEngine::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::risk
