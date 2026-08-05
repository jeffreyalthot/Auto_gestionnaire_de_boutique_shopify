#include "elit21/domain/SupplierId.h"

namespace elit21::domain {

SupplierId::SupplierId()
    : platform::BusinessComponent(
          "SupplierId",
          "commerce domain value object",
          platform::BusinessComponentSpec{
              "domain",
              "model_operation",
              std::vector<std::string>{},
              false,
              false,
              4U * 1024U * 1024U}) {}

platform::OperationResult SupplierId::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::domain
