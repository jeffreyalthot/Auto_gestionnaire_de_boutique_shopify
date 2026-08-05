#include "elit21/aliexpress/responses/DsProductGetResponse.h"

namespace elit21::aliexpress::responses {

DsProductGetResponse::DsProductGetResponse()
    : platform::BusinessComponent(
          "DsProductGetResponse",
          "AliExpress Open Platform dropshipping integration (responses)",
          platform::BusinessComponentSpec{
              "aliexpress",
              "query",
              std::vector<std::string>{},
              false,
              false,
              4U * 1024U * 1024U}) {}

platform::OperationResult DsProductGetResponse::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::aliexpress::responses
