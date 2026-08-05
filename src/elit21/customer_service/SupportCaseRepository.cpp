#include "elit21/customer_service/SupportCaseRepository.h"

namespace elit21::customer_service {

SupportCaseRepository::SupportCaseRepository()
    : platform::BusinessComponent(
          "SupportCaseRepository",
          "bilingual customer communication workflow",
          platform::BusinessComponentSpec{
              "customer_service",
              "model_operation",
              std::vector<std::string>{},
              false,
              false,
              4U * 1024U * 1024U}) {}

platform::OperationResult SupportCaseRepository::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::customer_service
