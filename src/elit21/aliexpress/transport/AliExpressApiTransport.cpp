#include "elit21/aliexpress/transport/AliExpressApiTransport.h"

namespace elit21::aliexpress::transport {

AliExpressApiTransport::AliExpressApiTransport()
    : platform::BusinessComponent(
          "AliExpressApiTransport",
          "AliExpress Open Platform dropshipping integration (transport)",
          platform::BusinessComponentSpec{
              "aliexpress",
              "model_operation",
              std::vector<std::string>{},
              false,
              false,
              4U * 1024U * 1024U}) {}

platform::OperationResult AliExpressApiTransport::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::aliexpress::transport
