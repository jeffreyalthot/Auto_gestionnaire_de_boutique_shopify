#include "elit21/aliexpress/services/AliExpressMemberReportingService.h"

namespace elit21::aliexpress::services {

AliExpressMemberReportingService::AliExpressMemberReportingService()
    : platform::BusinessComponent(
          "AliExpressMemberReportingService",
          "AliExpress Open Platform dropshipping integration (services)",
          platform::BusinessComponentSpec{
              "aliexpress",
              "orchestrate",
              std::vector<std::string>{},
              false,
              false,
              4U * 1024U * 1024U}) {}

platform::OperationResult AliExpressMemberReportingService::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::aliexpress::services
