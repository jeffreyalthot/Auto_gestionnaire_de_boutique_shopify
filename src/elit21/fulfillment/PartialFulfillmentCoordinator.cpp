#include "elit21/fulfillment/PartialFulfillmentCoordinator.h"

namespace elit21::fulfillment {

PartialFulfillmentCoordinator::PartialFulfillmentCoordinator()
    : platform::BusinessComponent(
          "PartialFulfillmentCoordinator",
          "shipment, tracking and Shopify fulfillment workflow",
          platform::BusinessComponentSpec{
              "fulfillment",
              "orchestrate",
              std::vector<std::string>{},
              true,
              false,
              4U * 1024U * 1024U}) {}

platform::OperationResult PartialFulfillmentCoordinator::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::fulfillment
