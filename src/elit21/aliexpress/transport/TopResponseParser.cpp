#include "elit21/aliexpress/transport/TopResponseParser.h"

namespace elit21::aliexpress::transport {

TopResponseParser::TopResponseParser()
    : platform::BusinessComponent(
          "TopResponseParser",
          "AliExpress Open Platform dropshipping integration (transport)",
          platform::BusinessComponentSpec{
              "aliexpress",
              "validate_transform",
              std::vector<std::string>{},
              false,
              false,
              4U * 1024U * 1024U}) {}

platform::OperationResult TopResponseParser::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::aliexpress::transport
