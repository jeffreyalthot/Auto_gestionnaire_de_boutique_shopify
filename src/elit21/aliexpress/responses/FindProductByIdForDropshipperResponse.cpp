#include "elit21/aliexpress/responses/FindProductByIdForDropshipperResponse.h"

namespace elit21::aliexpress::responses {

FindProductByIdForDropshipperResponse::FindProductByIdForDropshipperResponse()
    : platform::BusinessComponent(
          "FindProductByIdForDropshipperResponse",
          "AliExpress Open Platform dropshipping integration (responses)",
          platform::BusinessComponentSpec{
              "aliexpress",
              "query",
              std::vector<std::string>{},
              false,
              false,
              4U * 1024U * 1024U}) {}

platform::OperationResult FindProductByIdForDropshipperResponse::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::aliexpress::responses
