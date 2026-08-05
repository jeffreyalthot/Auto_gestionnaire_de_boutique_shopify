#include "elit21/catalog/ProductPublicationService.h"

namespace elit21::catalog {

ProductPublicationService::ProductPublicationService()
    : platform::BusinessComponent(
          "ProductPublicationService",
          "catalog normalization and publication",
          platform::BusinessComponentSpec{
              "catalog",
              "orchestrate",
              std::vector<std::string>{},
              true,
              false,
              4U * 1024U * 1024U}) {}

platform::OperationResult ProductPublicationService::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::catalog
