#include "elit21/aliexpress/services/AliExpressTrackingService.h"

namespace elit21::aliexpress::services {

AliExpressTrackingService::AliExpressTrackingService()
    : platform::BusinessComponent(
          "AliExpressTrackingService",
          "AliExpress Open Platform dropshipping integration (services)",
          platform::BusinessComponentSpec{
              "aliexpress",
              "orchestrate",
              std::vector<std::string>{},
              true,
              true,
              4U * 1024U * 1024U}) {}

platform::OperationResult AliExpressTrackingService::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::aliexpress::services
