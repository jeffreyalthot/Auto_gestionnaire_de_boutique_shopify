#include "elit21/aliexpress/responses/FreightCalculateResponse.h"

namespace elit21::aliexpress::responses {

FreightCalculateResponse::FreightCalculateResponse()
    : platform::BusinessComponent(
          "FreightCalculateResponse",
          "AliExpress Open Platform dropshipping integration (responses)",
          platform::BusinessComponentSpec{
              "aliexpress",
              "model_operation",
              std::vector<std::string>{},
              false,
              false,
              4U * 1024U * 1024U}) {}

platform::OperationResult FreightCalculateResponse::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::aliexpress::responses
