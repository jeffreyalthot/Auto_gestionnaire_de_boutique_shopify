#include "elit21/risk/SupplierRiskMonitor.h"

namespace elit21::risk {

SupplierRiskMonitor::SupplierRiskMonitor()
    : platform::BusinessComponent(
          "SupplierRiskMonitor",
          "fraud, loss and supplier risk controls",
          platform::BusinessComponentSpec{
              "risk",
              "query",
              std::vector<std::string>{},
              false,
              false,
              4U * 1024U * 1024U}) {}

platform::OperationResult SupplierRiskMonitor::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::risk
