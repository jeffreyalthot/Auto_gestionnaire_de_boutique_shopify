#include "elit21/domain/Percentage.h"

namespace elit21::domain {

Percentage::Percentage()
    : platform::BusinessComponent(
          "Percentage",
          "commerce domain value object",
          platform::BusinessComponentSpec{
              "domain",
              "model_operation",
              std::vector<std::string>{},
              false,
              false,
              4U * 1024U * 1024U}) {}

platform::OperationResult Percentage::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::domain
