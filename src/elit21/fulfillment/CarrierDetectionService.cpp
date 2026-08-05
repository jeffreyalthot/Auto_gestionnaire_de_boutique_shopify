#include "elit21/fulfillment/CarrierDetectionService.h"

namespace elit21::fulfillment {

CarrierDetectionService::CarrierDetectionService()
    : platform::BusinessComponent(
          "CarrierDetectionService",
          "shipment, tracking and Shopify fulfillment workflow",
          platform::BusinessComponentSpec{
              "fulfillment",
              "orchestrate",
              std::vector<std::string>{},
              true,
              false,
              4U * 1024U * 1024U}) {}

platform::OperationResult CarrierDetectionService::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::fulfillment
