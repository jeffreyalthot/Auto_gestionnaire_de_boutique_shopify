#include "elit21/catalog/VariantBuilder.h"

namespace elit21::catalog {

VariantBuilder::VariantBuilder()
    : platform::BusinessComponent(
          "VariantBuilder",
          "catalog normalization and publication",
          platform::BusinessComponentSpec{
              "catalog",
              "validate_transform",
              std::vector<std::string>{},
              false,
              false,
              4U * 1024U * 1024U}) {}

platform::OperationResult VariantBuilder::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::catalog
