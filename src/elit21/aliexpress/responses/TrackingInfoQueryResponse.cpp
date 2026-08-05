#include "elit21/aliexpress/responses/TrackingInfoQueryResponse.h"

namespace elit21::aliexpress::responses {

TrackingInfoQueryResponse::TrackingInfoQueryResponse()
    : platform::BusinessComponent(
          "TrackingInfoQueryResponse",
          "AliExpress Open Platform dropshipping integration (responses)",
          platform::BusinessComponentSpec{
              "aliexpress",
              "query",
              std::vector<std::string>{},
              false,
              false,
              4U * 1024U * 1024U}) {}

platform::OperationResult TrackingInfoQueryResponse::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::aliexpress::responses
