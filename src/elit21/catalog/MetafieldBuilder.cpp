#include "elit21/catalog/MetafieldBuilder.h"

namespace elit21::catalog {

MetafieldBuilder::MetafieldBuilder()
    : platform::BusinessComponent(
          "MetafieldBuilder",
          "catalog normalization and publication",
          platform::BusinessComponentSpec{
              "catalog",
              "validate_transform",
              std::vector<std::string>{},
              false,
              false,
              4U * 1024U * 1024U}) {}

platform::OperationResult MetafieldBuilder::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::catalog
