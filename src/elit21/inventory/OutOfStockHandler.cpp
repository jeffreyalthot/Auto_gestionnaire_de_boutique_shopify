#include "elit21/inventory/OutOfStockHandler.h"

namespace elit21::inventory {

OutOfStockHandler::OutOfStockHandler()
    : platform::BusinessComponent(
          "OutOfStockHandler",
          "inventory synchronization and oversell prevention",
          platform::BusinessComponentSpec{
              "inventory",
              "model_operation",
              std::vector<std::string>{},
              false,
              false,
              4U * 1024U * 1024U}) {}

platform::OperationResult OutOfStockHandler::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::inventory
