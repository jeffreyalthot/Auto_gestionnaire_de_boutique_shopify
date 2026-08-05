#include "elit21/mapping/FulfillmentStatusMapper.h"

namespace elit21::mapping {

FulfillmentStatusMapper::FulfillmentStatusMapper()
    : platform::BusinessComponent(
          "FulfillmentStatusMapper",
          "Shopify to AliExpress entity mapping",
          platform::BusinessComponentSpec{
              "mapping",
              "validate_transform",
              std::vector<std::string>{"fulfillment_order_id"},
              false,
              false,
              4U * 1024U * 1024U}) {}

platform::OperationResult FulfillmentStatusMapper::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::mapping
