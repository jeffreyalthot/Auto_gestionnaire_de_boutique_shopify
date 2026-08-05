#include "elit21/pricing/PriceHistoryService.h"

namespace elit21::pricing {

PriceHistoryService::PriceHistoryService()
    : platform::BusinessComponent(
          "PriceHistoryService",
          "100 percent markup before shipping pricing enforcement",
          platform::BusinessComponentSpec{
              "pricing",
              "orchestrate",
              std::vector<std::string>{},
              true,
              false,
              4U * 1024U * 1024U}) {}

platform::OperationResult PriceHistoryService::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::pricing
