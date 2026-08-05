#include "elit21/aliexpress/responses/DsRecommendFeedGetResponse.h"

namespace elit21::aliexpress::responses {

DsRecommendFeedGetResponse::DsRecommendFeedGetResponse()
    : platform::BusinessComponent(
          "DsRecommendFeedGetResponse",
          "AliExpress Open Platform dropshipping integration (responses)",
          platform::BusinessComponentSpec{
              "aliexpress",
              "query",
              std::vector<std::string>{},
              false,
              false,
              4U * 1024U * 1024U}) {}

platform::OperationResult DsRecommendFeedGetResponse::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::aliexpress::responses
