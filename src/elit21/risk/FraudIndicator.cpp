#include "elit21/risk/FraudIndicator.h"

namespace elit21::risk {

FraudIndicator::FraudIndicator()
    : platform::BusinessComponent(
          "FraudIndicator",
          "fraud, loss and supplier risk controls",
          platform::BusinessComponentSpec{
              "risk",
              "model_operation",
              std::vector<std::string>{},
              false,
              false,
              4U * 1024U * 1024U}) {}

platform::OperationResult FraudIndicator::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::risk
