#include "elit21/inventory/StockPollingPolicy.h"

namespace elit21::inventory {

StockPollingPolicy::StockPollingPolicy()
    : platform::BusinessComponent(
          "StockPollingPolicy",
          "inventory synchronization and oversell prevention",
          platform::BusinessComponentSpec{
              "inventory",
              "validate_transform",
              std::vector<std::string>{"sku"},
              false,
              false,
              4U * 1024U * 1024U}) {}

platform::OperationResult StockPollingPolicy::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::inventory
