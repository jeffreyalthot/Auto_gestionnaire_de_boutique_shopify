#include "elit21/domain/Money.h"

namespace elit21::domain {

Money::Money()
    : platform::BusinessComponent(
          "Money",
          "commerce domain value object",
          platform::BusinessComponentSpec{
              "domain",
              "model_operation",
              std::vector<std::string>{},
              false,
              false,
              4U * 1024U * 1024U}) {}

platform::OperationResult Money::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::domain
