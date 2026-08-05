#include "elit21/orders/AutomaticOrderProcessor.h"

namespace elit21::orders {

AutomaticOrderProcessor::AutomaticOrderProcessor()
    : platform::BusinessComponent(
          "AutomaticOrderProcessor",
          "order validation and supplier placement workflow",
          platform::BusinessComponentSpec{
              "orders",
              "command",
              std::vector<std::string>{"order_id"},
              true,
              false,
              4U * 1024U * 1024U}) {}

platform::OperationResult AutomaticOrderProcessor::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::orders
