#include "elit21/risk/TransactionRiskAnalyzer.h"

namespace elit21::risk {

TransactionRiskAnalyzer::TransactionRiskAnalyzer()
    : platform::BusinessComponent(
          "TransactionRiskAnalyzer",
          "fraud, loss and supplier risk controls",
          platform::BusinessComponentSpec{
              "risk",
              "query",
              std::vector<std::string>{},
              false,
              false,
              4U * 1024U * 1024U}) {}

platform::OperationResult TransactionRiskAnalyzer::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::risk
