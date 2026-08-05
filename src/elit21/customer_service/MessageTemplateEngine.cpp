#include "elit21/customer_service/MessageTemplateEngine.h"

namespace elit21::customer_service {

MessageTemplateEngine::MessageTemplateEngine()
    : platform::BusinessComponent(
          "MessageTemplateEngine",
          "bilingual customer communication workflow",
          platform::BusinessComponentSpec{
              "customer_service",
              "query",
              std::vector<std::string>{},
              false,
              false,
              4U * 1024U * 1024U}) {}

platform::OperationResult MessageTemplateEngine::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::customer_service
