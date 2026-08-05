#include "elit21/fulfillment/ShipmentMonitor.h"

namespace elit21::fulfillment {

ShipmentMonitor::ShipmentMonitor()
    : platform::BusinessComponent(
          "ShipmentMonitor",
          "shipment, tracking and Shopify fulfillment workflow",
          platform::BusinessComponentSpec{
              "fulfillment",
              "query",
              std::vector<std::string>{},
              false,
              false,
              4U * 1024U * 1024U}) {}

platform::OperationResult ShipmentMonitor::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::fulfillment
