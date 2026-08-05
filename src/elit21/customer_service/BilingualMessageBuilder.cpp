#include "elit21/customer_service/BilingualMessageBuilder.h"

namespace elit21::customer_service {

BilingualMessageBuilder::BilingualMessageBuilder()
    : platform::BusinessComponent(
          "BilingualMessageBuilder",
          "bilingual customer communication workflow",
          platform::BusinessComponentSpec{
              "customer_service",
              "validate_transform",
              std::vector<std::string>{},
              false,
              false,
              4U * 1024U * 1024U}) {}

platform::OperationResult BilingualMessageBuilder::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::customer_service
