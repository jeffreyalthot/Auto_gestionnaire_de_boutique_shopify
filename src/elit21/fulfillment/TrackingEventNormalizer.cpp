#include "elit21/fulfillment/TrackingEventNormalizer.h"

namespace elit21::fulfillment {

TrackingEventNormalizer::TrackingEventNormalizer()
    : platform::BusinessComponent(
          "TrackingEventNormalizer",
          "shipment, tracking and Shopify fulfillment workflow",
          platform::BusinessComponentSpec{
              "fulfillment",
              "validate_transform",
              std::vector<std::string>{"tracking_number"},
              false,
              false,
              4U * 1024U * 1024U}) {}

platform::OperationResult TrackingEventNormalizer::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::fulfillment
