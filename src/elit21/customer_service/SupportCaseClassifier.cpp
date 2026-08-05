#include "elit21/customer_service/SupportCaseClassifier.h"

namespace elit21::customer_service {

SupportCaseClassifier::SupportCaseClassifier()
    : platform::BusinessComponent(
          "SupportCaseClassifier",
          "bilingual customer communication workflow",
          platform::BusinessComponentSpec{
              "customer_service",
              "validate_transform",
              std::vector<std::string>{},
              false,
              false,
              4U * 1024U * 1024U}) {}

platform::OperationResult SupportCaseClassifier::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::customer_service
