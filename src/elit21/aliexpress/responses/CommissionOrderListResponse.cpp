#include "elit21/aliexpress/responses/CommissionOrderListResponse.h"

namespace elit21::aliexpress::responses {

CommissionOrderListResponse::CommissionOrderListResponse()
    : platform::BusinessComponent(
          "CommissionOrderListResponse",
          "AliExpress Open Platform dropshipping integration (responses)",
          platform::BusinessComponentSpec{
              "aliexpress",
              "query",
              std::vector<std::string>{},
              false,
              false,
              4U * 1024U * 1024U}) {}

platform::OperationResult CommissionOrderListResponse::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::aliexpress::responses
