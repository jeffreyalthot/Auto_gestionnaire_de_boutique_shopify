#include "elit21/catalog/ProductContentNormalizer.h"

namespace elit21::catalog {

ProductContentNormalizer::ProductContentNormalizer()
    : platform::BusinessComponent(
          "ProductContentNormalizer",
          "catalog normalization and publication",
          platform::BusinessComponentSpec{
              "catalog",
              "validate_transform",
              std::vector<std::string>{"product_id"},
              false,
              false,
              4U * 1024U * 1024U}) {}

platform::OperationResult ProductContentNormalizer::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::catalog
