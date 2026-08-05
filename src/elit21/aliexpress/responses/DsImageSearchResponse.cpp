#include "elit21/aliexpress/responses/DsImageSearchResponse.h"

namespace elit21::aliexpress::responses {

DsImageSearchResponse::DsImageSearchResponse()
    : platform::BusinessComponent(
          "DsImageSearchResponse",
          "AliExpress Open Platform dropshipping integration (responses)",
          platform::BusinessComponentSpec{
              "aliexpress",
              "query",
              std::vector<std::string>{},
              false,
              false,
              4U * 1024U * 1024U}) {}

platform::OperationResult DsImageSearchResponse::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::aliexpress::responses
