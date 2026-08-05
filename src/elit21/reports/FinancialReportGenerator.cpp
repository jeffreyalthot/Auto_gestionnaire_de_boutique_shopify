#include "elit21/reports/FinancialReportGenerator.h"

namespace elit21::reports {

FinancialReportGenerator::FinancialReportGenerator()
    : platform::BusinessComponent(
          "FinancialReportGenerator",
          "scheduled operational and financial reporting",
          platform::BusinessComponentSpec{
              "reports",
              "model_operation",
              std::vector<std::string>{},
              false,
              false,
              4U * 1024U * 1024U}) {}

platform::OperationResult FinancialReportGenerator::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::reports
