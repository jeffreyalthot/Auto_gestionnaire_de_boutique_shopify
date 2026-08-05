#include "elit21/fulfillment/TrackingPollingPolicy.h"

namespace elit21::fulfillment {

TrackingPollingPolicy::TrackingPollingPolicy()
    : platform::BusinessComponent(
          "TrackingPollingPolicy",
          "shipment, tracking and Shopify fulfillment workflow",
          platform::BusinessComponentSpec{
              "fulfillment",
              "validate_transform",
              std::vector<std::string>{"tracking_number"},
              false,
              false,
              4U * 1024U * 1024U}) {}

platform::OperationResult TrackingPollingPolicy::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::fulfillment
