#include "elit21/fulfillment/FulfillmentReconciliationService.h"

namespace elit21::fulfillment {

FulfillmentReconciliationService::FulfillmentReconciliationService()
    : platform::BusinessComponent(
          "FulfillmentReconciliationService",
          "shipment, tracking and Shopify fulfillment workflow",
          platform::BusinessComponentSpec{
              "fulfillment",
              "orchestrate",
              std::vector<std::string>{},
              true,
              false,
              4U * 1024U * 1024U}) {}

platform::OperationResult FulfillmentReconciliationService::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::fulfillment
