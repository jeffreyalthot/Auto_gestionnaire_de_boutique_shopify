#include "elit21/aliexpress/services/AliExpressImageSearchService.h"

namespace elit21::aliexpress::services {

AliExpressImageSearchService::AliExpressImageSearchService()
    : platform::BusinessComponent(
          "AliExpressImageSearchService",
          "AliExpress Open Platform dropshipping integration (services)",
          platform::BusinessComponentSpec{
              "aliexpress",
              "query",
              std::vector<std::string>{},
              false,
              false,
              4U * 1024U * 1024U}) {}

platform::OperationResult AliExpressImageSearchService::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::aliexpress::services
