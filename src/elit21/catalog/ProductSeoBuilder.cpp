#include "elit21/catalog/ProductSeoBuilder.h"

namespace elit21::catalog {

ProductSeoBuilder::ProductSeoBuilder()
    : platform::BusinessComponent(
          "ProductSeoBuilder",
          "catalog normalization and publication",
          platform::BusinessComponentSpec{
              "catalog",
              "validate_transform",
              std::vector<std::string>{"product_id"},
              false,
              false,
              4U * 1024U * 1024U}) {}

platform::OperationResult ProductSeoBuilder::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::catalog
