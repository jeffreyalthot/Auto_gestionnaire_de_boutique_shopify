#include "elit21/domain/LanguageCode.h"

namespace elit21::domain {

LanguageCode::LanguageCode()
    : platform::BusinessComponent(
          "LanguageCode",
          "commerce domain value object",
          platform::BusinessComponentSpec{
              "domain",
              "model_operation",
              std::vector<std::string>{},
              false,
              false,
              4U * 1024U * 1024U}) {}

platform::OperationResult LanguageCode::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::domain
