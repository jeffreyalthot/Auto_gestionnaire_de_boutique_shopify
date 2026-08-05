#include "elit21/aliexpress/AliExpressServiceFactory.h"

namespace elit21::aliexpress {

AliExpressServiceFactory::AliExpressServiceFactory()
    : platform::BusinessComponent(
          "AliExpressServiceFactory",
          "AliExpress Open Platform dropshipping integration",
          platform::BusinessComponentSpec{
              "aliexpress",
              "orchestrate",
              std::vector<std::string>{},
              true,
              true,
              4U * 1024U * 1024U}) {}

platform::OperationResult AliExpressServiceFactory::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::aliexpress
