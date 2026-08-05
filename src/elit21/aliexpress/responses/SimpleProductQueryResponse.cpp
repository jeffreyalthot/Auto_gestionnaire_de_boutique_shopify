#include "elit21/aliexpress/responses/SimpleProductQueryResponse.h"

namespace elit21::aliexpress::responses {

SimpleProductQueryResponse::SimpleProductQueryResponse()
    : platform::BusinessComponent(
          "SimpleProductQueryResponse",
          "AliExpress Open Platform dropshipping integration (responses)",
          platform::BusinessComponentSpec{
              "aliexpress",
              "query",
              std::vector<std::string>{},
              false,
              false,
              4U * 1024U * 1024U}) {}

platform::OperationResult SimpleProductQueryResponse::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::aliexpress::responses
