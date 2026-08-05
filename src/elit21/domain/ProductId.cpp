#include "elit21/domain/ProductId.h"

namespace elit21::domain {

ProductId::ProductId()
    : platform::BusinessComponent(
          "ProductId",
          "commerce domain value object",
          platform::BusinessComponentSpec{
              "domain",
              "model_operation",
              std::vector<std::string>{},
              false,
              false,
              4U * 1024U * 1024U}) {}

platform::OperationResult ProductId::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::domain
