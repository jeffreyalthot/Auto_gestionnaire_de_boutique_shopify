#include "elit21/aliexpress/services/AliExpressProductService.h"

namespace elit21::aliexpress::services {

AliExpressProductService::AliExpressProductService()
    : platform::BusinessComponent(
          "AliExpressProductService",
          "AliExpress Open Platform dropshipping integration (services)",
          platform::BusinessComponentSpec{
              "aliexpress",
              "orchestrate",
              std::vector<std::string>{},
              true,
              true,
              4U * 1024U * 1024U}) {}

platform::OperationResult AliExpressProductService::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::aliexpress::services
