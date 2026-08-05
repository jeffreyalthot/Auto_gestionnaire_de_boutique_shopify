#include "elit21/aliexpress/AliExpressConfig.h"

namespace elit21::aliexpress {

AliExpressConfig::AliExpressConfig()
    : platform::BusinessComponent(
          "AliExpressConfig",
          "AliExpress Open Platform dropshipping integration",
          platform::BusinessComponentSpec{
              "aliexpress",
              "model_operation",
              std::vector<std::string>{},
              false,
              false,
              4U * 1024U * 1024U}) {}

platform::OperationResult AliExpressConfig::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::aliexpress
