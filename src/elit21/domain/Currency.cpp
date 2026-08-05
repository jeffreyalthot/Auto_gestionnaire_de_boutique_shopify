#include "elit21/domain/Currency.h"

namespace elit21::domain {

Currency::Currency()
    : platform::BusinessComponent(
          "Currency",
          "commerce domain value object",
          platform::BusinessComponentSpec{
              "domain",
              "model_operation",
              std::vector<std::string>{},
              false,
              false,
              4U * 1024U * 1024U}) {}

platform::OperationResult Currency::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::domain
