#include "elit21/domain/PhoneNumber.h"

namespace elit21::domain {

PhoneNumber::PhoneNumber()
    : platform::BusinessComponent(
          "PhoneNumber",
          "commerce domain value object",
          platform::BusinessComponentSpec{
              "domain",
              "model_operation",
              std::vector<std::string>{},
              false,
              false,
              4U * 1024U * 1024U}) {}

platform::OperationResult PhoneNumber::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::domain
