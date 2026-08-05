#include "elit21/customer_service/CustomerMessageDispatcher.h"

namespace elit21::customer_service {

CustomerMessageDispatcher::CustomerMessageDispatcher()
    : platform::BusinessComponent(
          "CustomerMessageDispatcher",
          "bilingual customer communication workflow",
          platform::BusinessComponentSpec{
              "customer_service",
              "command",
              std::vector<std::string>{},
              true,
              false,
              4U * 1024U * 1024U}) {}

platform::OperationResult CustomerMessageDispatcher::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::customer_service
