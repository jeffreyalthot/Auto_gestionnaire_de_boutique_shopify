#include "elit21/fulfillment/LostShipmentDetector.h"

namespace elit21::fulfillment {

LostShipmentDetector::LostShipmentDetector()
    : platform::BusinessComponent(
          "LostShipmentDetector",
          "shipment, tracking and Shopify fulfillment workflow",
          platform::BusinessComponentSpec{
              "fulfillment",
              "query",
              std::vector<std::string>{},
              false,
              false,
              4U * 1024U * 1024U}) {}

platform::OperationResult LostShipmentDetector::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::fulfillment
