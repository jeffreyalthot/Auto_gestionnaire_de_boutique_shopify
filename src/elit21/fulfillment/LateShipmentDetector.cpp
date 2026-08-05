#include "elit21/fulfillment/LateShipmentDetector.h"

namespace elit21::fulfillment {

LateShipmentDetector::LateShipmentDetector()
    : platform::BusinessComponent(
          "LateShipmentDetector",
          "shipment, tracking and Shopify fulfillment workflow",
          platform::BusinessComponentSpec{
              "fulfillment",
              "query",
              std::vector<std::string>{},
              false,
              false,
              4U * 1024U * 1024U}) {}

platform::OperationResult LateShipmentDetector::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::fulfillment
