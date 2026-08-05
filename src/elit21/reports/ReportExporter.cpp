#include "elit21/reports/ReportExporter.h"

namespace elit21::reports {

ReportExporter::ReportExporter()
    : platform::BusinessComponent(
          "ReportExporter",
          "scheduled operational and financial reporting",
          platform::BusinessComponentSpec{
              "reports",
              "command",
              std::vector<std::string>{},
              false,
              false,
              4U * 1024U * 1024U}) {}

platform::OperationResult ReportExporter::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::reports
