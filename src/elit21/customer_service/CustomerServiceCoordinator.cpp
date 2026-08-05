#include "elit21/customer_service/CustomerServiceCoordinator.h"

namespace elit21::customer_service {

CustomerServiceCoordinator::CustomerServiceCoordinator()
    : platform::BusinessComponent(
          "CustomerServiceCoordinator",
          "bilingual customer communication workflow",
          platform::BusinessComponentSpec{
              "customer_service",
              "orchestrate",
              std::vector<std::string>{},
              true,
              false,
              4U * 1024U * 1024U}) {}

platform::OperationResult CustomerServiceCoordinator::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::customer_service
