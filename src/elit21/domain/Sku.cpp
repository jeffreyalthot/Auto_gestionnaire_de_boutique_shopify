#include "elit21/domain/Sku.h"

namespace elit21::domain {

Sku::Sku()
    : platform::BusinessComponent(
          "Sku",
          "commerce domain value object",
          platform::BusinessComponentSpec{
              "domain",
              "model_operation",
              std::vector<std::string>{},
              false,
              false,
              4U * 1024U * 1024U}) {}

platform::OperationResult Sku::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::domain
