#include "elit21/aliexpress/AliExpressSession.h"

namespace elit21::aliexpress {

AliExpressSession::AliExpressSession()
    : platform::BusinessComponent(
          "AliExpressSession",
          "AliExpress Open Platform dropshipping integration",
          platform::BusinessComponentSpec{
              "aliexpress",
              "model_operation",
              std::vector<std::string>{},
              false,
              false,
              4U * 1024U * 1024U}) {}

platform::OperationResult AliExpressSession::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::aliexpress
