#include "elit21/security/WebhookReplayGuard.h"

namespace elit21::security {

WebhookReplayGuard::WebhookReplayGuard()
    : platform::BusinessComponent(
          "WebhookReplayGuard",
          "credential, signature and data protection",
          platform::BusinessComponentSpec{
              "security",
              "validate_transform",
              std::vector<std::string>{"topic"},
              false,
              false,
              4U * 1024U * 1024U}) {}

platform::OperationResult WebhookReplayGuard::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::security
