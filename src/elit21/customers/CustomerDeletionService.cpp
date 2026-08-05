#include "elit21/customers/CustomerDeletionService.h"

namespace elit21::customers {

CustomerDeletionService::CustomerDeletionService()
    : platform::BusinessComponent(
          "CustomerDeletionService",
          "customer data lifecycle management",
          platform::BusinessComponentSpec{
              "customers",
              "orchestrate",
              std::vector<std::string>{},
              true,
              false,
              4U * 1024U * 1024U}) {}

platform::OperationResult CustomerDeletionService::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::customers
