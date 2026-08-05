#include "elit21/fulfillment/TrackingSynchronizationService.h"

namespace elit21::fulfillment {

TrackingSynchronizationService::TrackingSynchronizationService()
    : platform::BusinessComponent(
          "TrackingSynchronizationService",
          "shipment, tracking and Shopify fulfillment workflow",
          platform::BusinessComponentSpec{
              "fulfillment",
              "command",
              std::vector<std::string>{"tracking_number"},
              true,
              false,
              4U * 1024U * 1024U}) {}

platform::OperationResult TrackingSynchronizationService::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::fulfillment
