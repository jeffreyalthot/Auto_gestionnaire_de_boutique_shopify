#include "elit21/domain/OrderId.h"

namespace elit21::domain {

OrderId::OrderId()
    : platform::BusinessComponent(
          "OrderId",
          "commerce domain value object",
          platform::BusinessComponentSpec{
              "domain",
              "model_operation",
              std::vector<std::string>{},
              false,
              false,
              4U * 1024U * 1024U}) {}

platform::OperationResult OrderId::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::domain
