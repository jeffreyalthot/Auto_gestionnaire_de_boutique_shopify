#include "elit21/customer_service/DelayNotificationService.h"

namespace elit21::customer_service {

DelayNotificationService::DelayNotificationService()
    : platform::BusinessComponent(
          "DelayNotificationService",
          "bilingual customer communication workflow",
          platform::BusinessComponentSpec{
              "customer_service",
              "orchestrate",
              std::vector<std::string>{},
              true,
              false,
              4U * 1024U * 1024U}) {}

platform::OperationResult DelayNotificationService::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::customer_service
