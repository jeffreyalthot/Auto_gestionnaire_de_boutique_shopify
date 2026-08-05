#include "elit21/catalog/VariantOptionNormalizer.h"

namespace elit21::catalog {

VariantOptionNormalizer::VariantOptionNormalizer()
    : platform::BusinessComponent(
          "VariantOptionNormalizer",
          "catalog normalization and publication",
          platform::BusinessComponentSpec{
              "catalog",
              "validate_transform",
              std::vector<std::string>{},
              false,
              false,
              4U * 1024U * 1024U}) {}

platform::OperationResult VariantOptionNormalizer::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::catalog
