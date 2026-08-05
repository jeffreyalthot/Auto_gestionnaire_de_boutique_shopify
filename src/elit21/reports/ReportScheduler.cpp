#include "elit21/reports/ReportScheduler.h"

namespace elit21::reports {

ReportScheduler::ReportScheduler()
    : platform::BusinessComponent(
          "ReportScheduler",
          "scheduled operational and financial reporting",
          platform::BusinessComponentSpec{
              "reports",
              "command",
              std::vector<std::string>{},
              false,
              false,
              4U * 1024U * 1024U}) {}

platform::OperationResult ReportScheduler::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::reports
