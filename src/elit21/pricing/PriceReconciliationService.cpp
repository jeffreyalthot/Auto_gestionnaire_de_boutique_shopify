#include "elit21/pricing/PriceReconciliationService.h"

namespace elit21::pricing {

PriceReconciliationService::PriceReconciliationService()
    : platform::BusinessComponent(
          "PriceReconciliationService",
          "100 percent markup before shipping pricing enforcement",
          platform::BusinessComponentSpec{
              "pricing",
              "orchestrate",
              std::vector<std::string>{},
              true,
              false,
              4U * 1024U * 1024U}) {}

platform::OperationResult PriceReconciliationService::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::pricing
