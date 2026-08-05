#include "elit21/finance/TaxReportExporter.h"

namespace elit21::finance {

TaxReportExporter::TaxReportExporter()
    : platform::BusinessComponent(
          "TaxReportExporter",
          "financial ledger and profitability reconciliation",
          platform::BusinessComponentSpec{
              "finance",
              "command",
              std::vector<std::string>{},
              false,
              false,
              4U * 1024U * 1024U}) {}

platform::OperationResult TaxReportExporter::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::finance
