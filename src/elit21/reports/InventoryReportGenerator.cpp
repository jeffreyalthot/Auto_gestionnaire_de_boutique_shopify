#include "elit21/reports/InventoryReportGenerator.h"

namespace elit21::reports {

InventoryReportGenerator::InventoryReportGenerator()
    : platform::BusinessComponent(
          "InventoryReportGenerator",
          "scheduled operational and financial reporting",
          platform::BusinessComponentSpec{
              "reports",
              "model_operation",
              std::vector<std::string>{},
              false,
              false,
              4U * 1024U * 1024U}) {}

platform::OperationResult InventoryReportGenerator::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::reports
