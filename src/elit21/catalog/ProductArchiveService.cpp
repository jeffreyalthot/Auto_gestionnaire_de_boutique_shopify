#include "elit21/catalog/ProductArchiveService.h"

namespace elit21::catalog {

ProductArchiveService::ProductArchiveService()
    : platform::BusinessComponent(
          "ProductArchiveService",
          "catalog normalization and publication",
          platform::BusinessComponentSpec{
              "catalog",
              "command",
              std::vector<std::string>{"product_id"},
              true,
              false,
              4U * 1024U * 1024U}) {}

platform::OperationResult ProductArchiveService::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::catalog
