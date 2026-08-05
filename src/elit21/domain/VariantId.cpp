#include "elit21/domain/VariantId.h"

namespace elit21::domain {

VariantId::VariantId()
    : platform::BusinessComponent(
          "VariantId",
          "commerce domain value object",
          platform::BusinessComponentSpec{
              "domain",
              "model_operation",
              std::vector<std::string>{},
              false,
              false,
              4U * 1024U * 1024U}) {}

platform::OperationResult VariantId::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::domain
