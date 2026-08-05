#include "elit21/domain/Address.h"

namespace elit21::domain {

Address::Address()
    : platform::BusinessComponent(
          "Address",
          "commerce domain value object",
          platform::BusinessComponentSpec{
              "domain",
              "command",
              std::vector<std::string>{},
              true,
              false,
              4U * 1024U * 1024U}) {}

platform::OperationResult Address::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::domain
