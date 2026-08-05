#include "elit21/catalog/ProductCreationPlan.h"

namespace elit21::catalog {

ProductCreationPlan::ProductCreationPlan()
    : platform::BusinessComponent(
          "ProductCreationPlan",
          "catalog normalization and publication",
          platform::BusinessComponentSpec{
              "catalog",
              "model_operation",
              std::vector<std::string>{},
              false,
              false,
              4U * 1024U * 1024U}) {}

platform::OperationResult ProductCreationPlan::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::catalog
