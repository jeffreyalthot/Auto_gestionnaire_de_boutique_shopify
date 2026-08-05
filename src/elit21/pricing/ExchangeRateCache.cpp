#include "elit21/pricing/ExchangeRateCache.h"

namespace elit21::pricing {

ExchangeRateCache::ExchangeRateCache()
    : platform::BusinessComponent(
          "ExchangeRateCache",
          "100 percent markup before shipping pricing enforcement",
          platform::BusinessComponentSpec{
              "pricing",
              "model_operation",
              std::vector<std::string>{},
              false,
              false,
              4U * 1024U * 1024U}) {}

platform::OperationResult ExchangeRateCache::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::pricing
