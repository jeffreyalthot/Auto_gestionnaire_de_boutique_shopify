#include "elit21/domain/EmailAddress.h"

namespace elit21::domain {

EmailAddress::EmailAddress()
    : platform::BusinessComponent(
          "EmailAddress",
          "commerce domain value object",
          platform::BusinessComponentSpec{
              "domain",
              "command",
              std::vector<std::string>{},
              true,
              false,
              4U * 1024U * 1024U}) {}

platform::OperationResult EmailAddress::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::domain
