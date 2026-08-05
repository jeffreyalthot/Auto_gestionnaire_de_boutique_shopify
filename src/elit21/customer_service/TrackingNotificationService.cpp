#include "elit21/customer_service/TrackingNotificationService.h"

namespace elit21::customer_service {

TrackingNotificationService::TrackingNotificationService()
    : platform::BusinessComponent(
          "TrackingNotificationService",
          "bilingual customer communication workflow",
          platform::BusinessComponentSpec{
              "customer_service",
              "orchestrate",
              std::vector<std::string>{},
              true,
              false,
              4U * 1024U * 1024U}) {}

platform::OperationResult TrackingNotificationService::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::customer_service
