#include "elit21/reports/OrderReportGenerator.h"

namespace elit21::reports {

OrderReportGenerator::OrderReportGenerator()
    : platform::BusinessComponent(
          "OrderReportGenerator",
          "scheduled operational and financial reporting",
          platform::BusinessComponentSpec{
              "reports",
              "model_operation",
              std::vector<std::string>{},
              false,
              false,
              4U * 1024U * 1024U}) {}

platform::OperationResult OrderReportGenerator::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::reports
