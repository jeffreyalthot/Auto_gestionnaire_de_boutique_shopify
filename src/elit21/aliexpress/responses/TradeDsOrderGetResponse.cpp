#include "elit21/aliexpress/responses/TradeDsOrderGetResponse.h"

namespace elit21::aliexpress::responses {

TradeDsOrderGetResponse::TradeDsOrderGetResponse()
    : platform::BusinessComponent(
          "TradeDsOrderGetResponse",
          "AliExpress Open Platform dropshipping integration (responses)",
          platform::BusinessComponentSpec{
              "aliexpress",
              "query",
              std::vector<std::string>{},
              false,
              false,
              4U * 1024U * 1024U}) {}

platform::OperationResult TradeDsOrderGetResponse::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::aliexpress::responses
