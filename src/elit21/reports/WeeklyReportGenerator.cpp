#include "elit21/reports/WeeklyReportGenerator.h"

namespace elit21::reports {

WeeklyReportGenerator::WeeklyReportGenerator()
    : platform::BusinessComponent(
          "WeeklyReportGenerator",
          "scheduled operational and financial reporting",
          platform::BusinessComponentSpec{
              "reports",
              "model_operation",
              std::vector<std::string>{},
              false,
              false,
              4U * 1024U * 1024U}) {}

platform::OperationResult WeeklyReportGenerator::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::reports
