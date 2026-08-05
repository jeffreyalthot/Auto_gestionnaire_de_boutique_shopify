#include "elit21/aliexpress/services/AliExpressRecommendationService.h"

namespace elit21::aliexpress::services {

AliExpressRecommendationService::AliExpressRecommendationService()
    : platform::BusinessComponent(
          "AliExpressRecommendationService",
          "AliExpress Open Platform dropshipping integration (services)",
          platform::BusinessComponentSpec{
              "aliexpress",
              "orchestrate",
              std::vector<std::string>{},
              true,
              true,
              4U * 1024U * 1024U}) {}

platform::OperationResult AliExpressRecommendationService::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::aliexpress::services
