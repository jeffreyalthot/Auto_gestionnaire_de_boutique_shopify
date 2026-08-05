#include "elit21/customers/CustomerRiskProfile.h"

namespace elit21::customers {

CustomerRiskProfile::CustomerRiskProfile()
    : platform::BusinessComponent(
          "CustomerRiskProfile",
          "customer data lifecycle management",
          platform::BusinessComponentSpec{
              "customers",
              "model_operation",
              std::vector<std::string>{},
              false,
              false,
              4U * 1024U * 1024U}) {}

platform::OperationResult CustomerRiskProfile::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::customers
