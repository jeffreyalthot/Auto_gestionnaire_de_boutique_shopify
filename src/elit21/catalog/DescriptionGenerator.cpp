#include "elit21/catalog/DescriptionGenerator.h"

namespace elit21::catalog {

DescriptionGenerator::DescriptionGenerator()
    : platform::BusinessComponent(
          "DescriptionGenerator",
          "catalog normalization and publication",
          platform::BusinessComponentSpec{
              "catalog",
              "model_operation",
              std::vector<std::string>{},
              false,
              false,
              4U * 1024U * 1024U}) {}

platform::OperationResult DescriptionGenerator::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::catalog
