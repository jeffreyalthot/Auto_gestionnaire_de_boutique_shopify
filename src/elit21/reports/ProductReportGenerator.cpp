#include "elit21/reports/ProductReportGenerator.h"

namespace elit21::reports {

ProductReportGenerator::ProductReportGenerator()
    : platform::BusinessComponent(
          "ProductReportGenerator",
          "scheduled operational and financial reporting",
          platform::BusinessComponentSpec{
              "reports",
              "model_operation",
              std::vector<std::string>{},
              false,
              false,
              4U * 1024U * 1024U}) {}

platform::OperationResult ProductReportGenerator::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::reports
