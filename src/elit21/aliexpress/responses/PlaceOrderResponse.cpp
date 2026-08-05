#include "elit21/aliexpress/responses/PlaceOrderResponse.h"

namespace elit21::aliexpress::responses {

PlaceOrderResponse::PlaceOrderResponse()
    : platform::BusinessComponent(
          "PlaceOrderResponse",
          "AliExpress Open Platform dropshipping integration (responses)",
          platform::BusinessComponentSpec{
              "aliexpress",
              "command",
              std::vector<std::string>{"order_id"},
              true,
              true,
              4U * 1024U * 1024U}) {}

platform::OperationResult PlaceOrderResponse::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::aliexpress::responses
