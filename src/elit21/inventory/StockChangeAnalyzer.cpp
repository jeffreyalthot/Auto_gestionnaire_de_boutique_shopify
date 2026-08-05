#include "elit21/inventory/StockChangeAnalyzer.h"

namespace elit21::inventory {

StockChangeAnalyzer::StockChangeAnalyzer()
    : platform::BusinessComponent(
          "StockChangeAnalyzer",
          "inventory synchronization and oversell prevention",
          platform::BusinessComponentSpec{
              "inventory",
              "query",
              std::vector<std::string>{},
              false,
              false,
              4U * 1024U * 1024U}) {}

platform::OperationResult StockChangeAnalyzer::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::inventory
