#include "elit21/aliexpress/services/AliExpressFreightService.h"

namespace elit21::aliexpress::services {

AliExpressFreightService::AliExpressFreightService()
    : platform::BusinessComponent(
          "AliExpressFreightService",
          "AliExpress Open Platform dropshipping integration (services)",
          platform::BusinessComponentSpec{
              "aliexpress",
              "orchestrate",
              std::vector<std::string>{},
              true,
              true,
              4U * 1024U * 1024U}) {}

platform::OperationResult AliExpressFreightService::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::aliexpress::services
