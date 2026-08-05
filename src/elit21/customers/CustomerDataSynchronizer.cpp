#include "elit21/customers/CustomerDataSynchronizer.h"

namespace elit21::customers {

CustomerDataSynchronizer::CustomerDataSynchronizer()
    : platform::BusinessComponent(
          "CustomerDataSynchronizer",
          "customer data lifecycle management",
          platform::BusinessComponentSpec{
              "customers",
              "command",
              std::vector<std::string>{"customer_id"},
              true,
              false,
              4U * 1024U * 1024U}) {}

platform::OperationResult CustomerDataSynchronizer::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::customers
