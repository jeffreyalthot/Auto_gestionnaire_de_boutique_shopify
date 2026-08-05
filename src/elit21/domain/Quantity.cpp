#include "elit21/domain/Quantity.h"

namespace elit21::domain {

Quantity::Quantity()
    : platform::BusinessComponent(
          "Quantity",
          "commerce domain value object",
          platform::BusinessComponentSpec{
              "domain",
              "model_operation",
              std::vector<std::string>{},
              false,
              false,
              4U * 1024U * 1024U}) {}

platform::OperationResult Quantity::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::domain
