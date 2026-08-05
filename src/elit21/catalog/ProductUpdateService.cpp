#include "elit21/catalog/ProductUpdateService.h"

namespace elit21::catalog {

ProductUpdateService::ProductUpdateService()
    : platform::BusinessComponent(
          "ProductUpdateService",
          "catalog normalization and publication",
          platform::BusinessComponentSpec{
              "catalog",
              "command",
              std::vector<std::string>{"product_id"},
              true,
              false,
              4U * 1024U * 1024U}) {}

platform::OperationResult ProductUpdateService::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::catalog
