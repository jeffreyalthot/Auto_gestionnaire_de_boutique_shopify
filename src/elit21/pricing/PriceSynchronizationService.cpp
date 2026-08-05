#include "elit21/pricing/PriceSynchronizationService.h"

namespace elit21::pricing {

PriceSynchronizationService::PriceSynchronizationService()
    : platform::BusinessComponent(
          "PriceSynchronizationService",
          "100 percent markup before shipping pricing enforcement",
          platform::BusinessComponentSpec{
              "pricing",
              "command",
              std::vector<std::string>{"supplier_cost_cad"},
              true,
              false,
              4U * 1024U * 1024U}) {}

platform::OperationResult PriceSynchronizationService::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::pricing
