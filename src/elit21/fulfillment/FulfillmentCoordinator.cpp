#include "elit21/fulfillment/FulfillmentCoordinator.h"

namespace elit21::fulfillment {

FulfillmentCoordinator::FulfillmentCoordinator()
    : platform::BusinessComponent(
          "FulfillmentCoordinator",
          "shipment, tracking and Shopify fulfillment workflow",
          platform::BusinessComponentSpec{
              "fulfillment",
              "orchestrate",
              std::vector<std::string>{},
              true,
              false,
              4U * 1024U * 1024U}) {}

platform::OperationResult FulfillmentCoordinator::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::fulfillment
