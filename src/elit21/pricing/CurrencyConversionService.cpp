#include "elit21/pricing/CurrencyConversionService.h"

namespace elit21::pricing {

CurrencyConversionService::CurrencyConversionService()
    : platform::BusinessComponent(
          "CurrencyConversionService",
          "100 percent markup before shipping pricing enforcement",
          platform::BusinessComponentSpec{
              "pricing",
              "orchestrate",
              std::vector<std::string>{},
              true,
              false,
              4U * 1024U * 1024U}) {}

platform::OperationResult CurrencyConversionService::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::pricing
