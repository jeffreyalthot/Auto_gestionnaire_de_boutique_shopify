#include "elit21/domain/CustomerId.h"

namespace elit21::domain {

CustomerId::CustomerId()
    : platform::BusinessComponent(
          "CustomerId",
          "commerce domain value object",
          platform::BusinessComponentSpec{
              "domain",
              "model_operation",
              std::vector<std::string>{},
              false,
              false,
              4U * 1024U * 1024U}) {}

platform::OperationResult CustomerId::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::domain
