#include "elit21/reports/Report.h"

namespace elit21::reports {

Report::Report()
    : platform::BusinessComponent(
          "Report",
          "scheduled operational and financial reporting",
          platform::BusinessComponentSpec{
              "reports",
              "model_operation",
              std::vector<std::string>{},
              false,
              false,
              4U * 1024U * 1024U}) {}

platform::OperationResult Report::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::reports
