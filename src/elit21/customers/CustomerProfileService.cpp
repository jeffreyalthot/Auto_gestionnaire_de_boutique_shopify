#include "elit21/customers/CustomerProfileService.h"

namespace elit21::customers {

CustomerProfileService::CustomerProfileService()
    : platform::BusinessComponent(
          "CustomerProfileService",
          "customer data lifecycle management",
          platform::BusinessComponentSpec{
              "customers",
              "orchestrate",
              std::vector<std::string>{},
              true,
              false,
              4U * 1024U * 1024U}) {}

platform::OperationResult CustomerProfileService::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::customers
