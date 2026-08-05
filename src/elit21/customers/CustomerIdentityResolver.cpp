#include "elit21/customers/CustomerIdentityResolver.h"

namespace elit21::customers {

CustomerIdentityResolver::CustomerIdentityResolver()
    : platform::BusinessComponent(
          "CustomerIdentityResolver",
          "customer data lifecycle management",
          platform::BusinessComponentSpec{
              "customers",
              "validate_transform",
              std::vector<std::string>{"customer_id"},
              false,
              false,
              4U * 1024U * 1024U}) {}

platform::OperationResult CustomerIdentityResolver::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::customers
