#include "elit21/aliexpress/responses/DsAddInfoResponse.h"

namespace elit21::aliexpress::responses {

DsAddInfoResponse::DsAddInfoResponse()
    : platform::BusinessComponent(
          "DsAddInfoResponse",
          "AliExpress Open Platform dropshipping integration (responses)",
          platform::BusinessComponentSpec{
              "aliexpress",
              "command",
              std::vector<std::string>{},
              true,
              true,
              4U * 1024U * 1024U}) {}

platform::OperationResult DsAddInfoResponse::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::aliexpress::responses
