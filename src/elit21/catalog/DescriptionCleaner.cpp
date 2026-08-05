#include "elit21/catalog/DescriptionCleaner.h"

namespace elit21::catalog {

DescriptionCleaner::DescriptionCleaner()
    : platform::BusinessComponent(
          "DescriptionCleaner",
          "catalog normalization and publication",
          platform::BusinessComponentSpec{
              "catalog",
              "model_operation",
              std::vector<std::string>{},
              false,
              false,
              4U * 1024U * 1024U}) {}

platform::OperationResult DescriptionCleaner::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::catalog
