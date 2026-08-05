#include "elit21/fulfillment/FulfillmentCreationService.h"

namespace elit21::fulfillment {

FulfillmentCreationService::FulfillmentCreationService()
    : platform::BusinessComponent(
          "FulfillmentCreationService",
          "shipment, tracking and Shopify fulfillment workflow",
          platform::BusinessComponentSpec{
              "fulfillment",
              "orchestrate",
              std::vector<std::string>{},
              true,
              false,
              4U * 1024U * 1024U}) {}

platform::OperationResult FulfillmentCreationService::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::fulfillment
