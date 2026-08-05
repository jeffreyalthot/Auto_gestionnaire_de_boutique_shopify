#include "elit21/aliexpress/services/AliExpressSupplierAccountService.h"

namespace elit21::aliexpress::services {

AliExpressSupplierAccountService::AliExpressSupplierAccountService()
    : platform::BusinessComponent(
          "AliExpressSupplierAccountService",
          "AliExpress Open Platform dropshipping integration (services)",
          platform::BusinessComponentSpec{
              "aliexpress",
              "orchestrate",
              std::vector<std::string>{},
              true,
              true,
              4U * 1024U * 1024U}) {}

platform::OperationResult AliExpressSupplierAccountService::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::aliexpress::services
