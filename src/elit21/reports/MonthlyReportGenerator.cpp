#include "elit21/reports/MonthlyReportGenerator.h"

namespace elit21::reports {

MonthlyReportGenerator::MonthlyReportGenerator()
    : platform::BusinessComponent(
          "MonthlyReportGenerator",
          "scheduled operational and financial reporting",
          platform::BusinessComponentSpec{
              "reports",
              "model_operation",
              std::vector<std::string>{},
              false,
              false,
              4U * 1024U * 1024U}) {}

platform::OperationResult MonthlyReportGenerator::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::reports
