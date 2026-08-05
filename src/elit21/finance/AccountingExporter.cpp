#include "elit21/finance/AccountingExporter.h"

namespace elit21::finance {

AccountingExporter::AccountingExporter()
    : platform::BusinessComponent(
          "AccountingExporter",
          "financial ledger and profitability reconciliation",
          platform::BusinessComponentSpec{
              "finance",
              "command",
              std::vector<std::string>{},
              true,
              false,
              4U * 1024U * 1024U}) {}

platform::OperationResult AccountingExporter::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::finance
