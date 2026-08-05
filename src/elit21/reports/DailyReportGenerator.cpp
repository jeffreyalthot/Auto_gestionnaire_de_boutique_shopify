#include "elit21/reports/DailyReportGenerator.h"

namespace elit21::reports {

DailyReportGenerator::DailyReportGenerator()
    : platform::BusinessComponent(
          "DailyReportGenerator",
          "scheduled operational and financial reporting",
          platform::BusinessComponentSpec{
              "reports",
              "model_operation",
              std::vector<std::string>{},
              false,
              false,
              4U * 1024U * 1024U}) {}

platform::OperationResult DailyReportGenerator::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::reports
