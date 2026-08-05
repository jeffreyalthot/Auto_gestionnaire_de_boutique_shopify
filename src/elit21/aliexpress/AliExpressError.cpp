#include "elit21/aliexpress/AliExpressError.h"

namespace elit21::aliexpress {

AliExpressError::AliExpressError()
    : platform::BusinessComponent(
          "AliExpressError",
          "AliExpress Open Platform dropshipping integration",
          platform::BusinessComponentSpec{
              "aliexpress",
              "model_operation",
              std::vector<std::string>{},
              false,
              false,
              4U * 1024U * 1024U}) {}

platform::OperationResult AliExpressError::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::aliexpress
