#include "elit21/domain/CountryCode.h"

namespace elit21::domain {

CountryCode::CountryCode()
    : platform::BusinessComponent(
          "CountryCode",
          "commerce domain value object",
          platform::BusinessComponentSpec{
              "domain",
              "model_operation",
              std::vector<std::string>{},
              false,
              false,
              4U * 1024U * 1024U}) {}

platform::OperationResult CountryCode::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::domain
