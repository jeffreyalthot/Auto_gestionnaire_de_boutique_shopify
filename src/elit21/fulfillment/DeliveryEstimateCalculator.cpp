#include "elit21/fulfillment/DeliveryEstimateCalculator.h"

namespace elit21::fulfillment {

DeliveryEstimateCalculator::DeliveryEstimateCalculator()
    : platform::BusinessComponent(
          "DeliveryEstimateCalculator",
          "shipment, tracking and Shopify fulfillment workflow",
          platform::BusinessComponentSpec{
              "fulfillment",
              "validate_transform",
              std::vector<std::string>{},
              false,
              false,
              4U * 1024U * 1024U}) {}

platform::OperationResult DeliveryEstimateCalculator::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::fulfillment
