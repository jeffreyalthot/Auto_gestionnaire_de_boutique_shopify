#include "elit21/mapping/CurrencyMapper.h"

namespace elit21::mapping {

CurrencyMapper::CurrencyMapper()
    : platform::BusinessComponent(
          "CurrencyMapper",
          "Shopify to AliExpress entity mapping",
          platform::BusinessComponentSpec{
              "mapping",
              "validate_transform",
              std::vector<std::string>{},
              false,
              false,
              4U * 1024U * 1024U}) {}

platform::OperationResult CurrencyMapper::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::mapping
