#include "elit21/catalog/CatalogReconciliationService.h"

namespace elit21::catalog {

CatalogReconciliationService::CatalogReconciliationService()
    : platform::BusinessComponent(
          "CatalogReconciliationService",
          "catalog normalization and publication",
          platform::BusinessComponentSpec{
              "catalog",
              "orchestrate",
              std::vector<std::string>{},
              true,
              false,
              4U * 1024U * 1024U}) {}

platform::OperationResult CatalogReconciliationService::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::catalog
