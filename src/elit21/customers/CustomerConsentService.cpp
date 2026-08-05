#include "elit21/customers/CustomerConsentService.h"

namespace elit21::customers {

CustomerConsentService::CustomerConsentService()
    : platform::BusinessComponent(
          "CustomerConsentService",
          "customer data lifecycle management",
          platform::BusinessComponentSpec{
              "customers",
              "orchestrate",
              std::vector<std::string>{},
              true,
              false,
              4U * 1024U * 1024U}) {}

platform::OperationResult CustomerConsentService::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::customers
