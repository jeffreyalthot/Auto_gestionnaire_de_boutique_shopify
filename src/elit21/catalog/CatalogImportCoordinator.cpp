#include "elit21/catalog/CatalogImportCoordinator.h"

namespace elit21::catalog {

CatalogImportCoordinator::CatalogImportCoordinator()
    : platform::BusinessComponent(
          "CatalogImportCoordinator",
          "catalog normalization and publication",
          platform::BusinessComponentSpec{
              "catalog",
              "command",
              std::vector<std::string>{},
              true,
              false,
              4U * 1024U * 1024U}) {}

platform::OperationResult CatalogImportCoordinator::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::catalog
