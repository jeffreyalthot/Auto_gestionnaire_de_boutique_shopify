#include "elit21/aliexpress/services/AliExpressCatalogService.h"

namespace elit21::aliexpress::services {

AliExpressCatalogService::AliExpressCatalogService()
    : platform::BusinessComponent(
          "AliExpressCatalogService",
          "AliExpress Open Platform dropshipping integration (services)",
          platform::BusinessComponentSpec{
              "aliexpress",
              "orchestrate",
              std::vector<std::string>{},
              true,
              true,
              4U * 1024U * 1024U}) {}

platform::OperationResult AliExpressCatalogService::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::aliexpress::services
