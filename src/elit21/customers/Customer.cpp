#include "elit21/customers/Customer.h"

namespace elit21::customers {

Customer::Customer()
    : platform::BusinessComponent(
          "Customer",
          "customer data lifecycle management",
          platform::BusinessComponentSpec{
              "customers",
              "model_operation",
              std::vector<std::string>{},
              false,
              false,
              4U * 1024U * 1024U}) {}

platform::OperationResult Customer::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::customers
