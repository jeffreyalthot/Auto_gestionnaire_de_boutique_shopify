#include "elit21/fulfillment/TrackingEvent.h"

namespace elit21::fulfillment {

TrackingEvent::TrackingEvent()
    : platform::BusinessComponent(
          "TrackingEvent",
          "shipment, tracking and Shopify fulfillment workflow",
          platform::BusinessComponentSpec{
              "fulfillment",
              "model_operation",
              std::vector<std::string>{},
              false,
              false,
              4U * 1024U * 1024U}) {}

platform::OperationResult TrackingEvent::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::fulfillment
