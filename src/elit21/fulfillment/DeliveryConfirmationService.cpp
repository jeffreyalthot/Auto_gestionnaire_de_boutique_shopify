#include "elit21/fulfillment/DeliveryConfirmationService.h"

namespace elit21::fulfillment {

DeliveryConfirmationService::DeliveryConfirmationService()
    : platform::BusinessComponent(
          "DeliveryConfirmationService",
          "shipment, tracking and Shopify fulfillment workflow",
          platform::BusinessComponentSpec{
              "fulfillment",
              "orchestrate",
              std::vector<std::string>{},
              true,
              false,
              4U * 1024U * 1024U}) {}

platform::OperationResult DeliveryConfirmationService::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::fulfillment
