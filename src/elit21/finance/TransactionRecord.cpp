#include "elit21/finance/TransactionRecord.h"

namespace elit21::finance {

TransactionRecord::TransactionRecord()
    : platform::BusinessComponent(
          "TransactionRecord",
          "financial ledger and profitability reconciliation",
          platform::BusinessComponentSpec{
              "finance",
              "model_operation",
              std::vector<std::string>{},
              false,
              false,
              4U * 1024U * 1024U}) {}

platform::OperationResult TransactionRecord::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::finance
