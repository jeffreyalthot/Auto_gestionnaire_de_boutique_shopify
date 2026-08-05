#include "elit21/catalog/ProductTitleBuilder.h"

namespace elit21::catalog {

ProductTitleBuilder::ProductTitleBuilder()
    : platform::BusinessComponent(
          "ProductTitleBuilder",
          "catalog normalization and publication",
          platform::BusinessComponentSpec{
              "catalog",
              "validate_transform",
              std::vector<std::string>{"product_id"},
              false,
              false,
              4U * 1024U * 1024U}) {}

platform::OperationResult ProductTitleBuilder::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::catalog
