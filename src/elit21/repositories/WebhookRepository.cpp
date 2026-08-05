#include "elit21/repositories/WebhookRepository.h"

namespace elit21::repositories {

WebhookRepository::WebhookRepository()
    : platform::BusinessComponent(
          "WebhookRepository",
          "persistent repository access",
          platform::BusinessComponentSpec{
              "repositories",
              "model_operation",
              std::vector<std::string>{},
              false,
              false,
              4U * 1024U * 1024U}) {}

platform::OperationResult WebhookRepository::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::repositories
