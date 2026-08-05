#include "elit21/aliexpress/transport/AliExpressRateLimiter.h"

namespace elit21::aliexpress::transport {

AliExpressRateLimiter::AliExpressRateLimiter()
    : platform::BusinessComponent(
          "AliExpressRateLimiter",
          "AliExpress Open Platform dropshipping integration (transport)",
          platform::BusinessComponentSpec{
              "aliexpress",
              "model_operation",
              std::vector<std::string>{},
              false,
              false,
              4U * 1024U * 1024U}) {}

platform::OperationResult AliExpressRateLimiter::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::aliexpress::transport
