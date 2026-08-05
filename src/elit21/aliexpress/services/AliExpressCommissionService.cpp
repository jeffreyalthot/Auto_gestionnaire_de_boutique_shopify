#include "elit21/aliexpress/services/AliExpressCommissionService.h"

namespace elit21::aliexpress::services {

AliExpressCommissionService::AliExpressCommissionService()
    : platform::BusinessComponent(
          "AliExpressCommissionService",
          "AliExpress Open Platform dropshipping integration (services)",
          platform::BusinessComponentSpec{
              "aliexpress",
              "orchestrate",
              std::vector<std::string>{},
              true,
              true,
              4U * 1024U * 1024U}) {}

platform::OperationResult AliExpressCommissionService::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::aliexpress::services
