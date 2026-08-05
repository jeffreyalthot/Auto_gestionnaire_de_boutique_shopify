#include "elit21/catalog/ProductImportService.h"

namespace elit21::catalog {

ProductImportService::ProductImportService()
    : platform::BusinessComponent(
          "ProductImportService",
          "catalog normalization and publication",
          platform::BusinessComponentSpec{
              "catalog",
              "command",
              std::vector<std::string>{"product_id"},
              true,
              false,
              4U * 1024U * 1024U}) {}

platform::OperationResult ProductImportService::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::catalog
