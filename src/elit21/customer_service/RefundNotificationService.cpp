#include "elit21/customer_service/RefundNotificationService.h"

namespace elit21::customer_service {

RefundNotificationService::RefundNotificationService()
    : platform::BusinessComponent(
          "RefundNotificationService",
          "bilingual customer communication workflow",
          platform::BusinessComponentSpec{
              "customer_service",
              "command",
              std::vector<std::string>{"order_id"},
              true,
              false,
              4U * 1024U * 1024U}) {}

platform::OperationResult RefundNotificationService::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::customer_service
