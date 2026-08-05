#include "elit21/customers/CustomerAnonymizationService.h"

namespace elit21::customers {

CustomerAnonymizationService::CustomerAnonymizationService()
    : platform::BusinessComponent(
          "CustomerAnonymizationService",
          "customer data lifecycle management",
          platform::BusinessComponentSpec{
              "customers",
              "orchestrate",
              std::vector<std::string>{},
              true,
              false,
              4U * 1024U * 1024U}) {}

platform::OperationResult CustomerAnonymizationService::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::customers
