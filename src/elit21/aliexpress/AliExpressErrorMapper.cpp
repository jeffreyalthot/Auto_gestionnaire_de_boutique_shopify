#include "elit21/aliexpress/AliExpressErrorMapper.h"

namespace elit21::aliexpress {

AliExpressErrorMapper::AliExpressErrorMapper()
    : platform::BusinessComponent(
          "AliExpressErrorMapper",
          "AliExpress Open Platform dropshipping integration",
          platform::BusinessComponentSpec{
              "aliexpress",
              "validate_transform",
              std::vector<std::string>{},
              false,
              false,
              4U * 1024U * 1024U}) {}

platform::OperationResult AliExpressErrorMapper::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::aliexpress
