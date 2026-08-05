#include "elit21/customer_service/EmailNotificationClient.h"

namespace elit21::customer_service {

EmailNotificationClient::EmailNotificationClient()
    : platform::BusinessComponent(
          "EmailNotificationClient",
          "bilingual customer communication workflow",
          platform::BusinessComponentSpec{
              "customer_service",
              "model_operation",
              std::vector<std::string>{},
              false,
              false,
              4U * 1024U * 1024U}) {}

platform::OperationResult EmailNotificationClient::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::customer_service
