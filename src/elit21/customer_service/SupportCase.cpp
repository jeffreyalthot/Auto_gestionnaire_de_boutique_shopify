#include "elit21/customer_service/SupportCase.h"

namespace elit21::customer_service {

SupportCase::SupportCase()
    : platform::BusinessComponent(
          "SupportCase",
          "bilingual customer communication workflow",
          platform::BusinessComponentSpec{
              "customer_service",
              "model_operation",
              std::vector<std::string>{},
              false,
              false,
              4U * 1024U * 1024U}) {}

platform::OperationResult SupportCase::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::customer_service
