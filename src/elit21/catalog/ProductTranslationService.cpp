#include "elit21/catalog/ProductTranslationService.h"

namespace elit21::catalog {

ProductTranslationService::ProductTranslationService()
    : platform::BusinessComponent(
          "ProductTranslationService",
          "catalog normalization and publication",
          platform::BusinessComponentSpec{
              "catalog",
              "orchestrate",
              std::vector<std::string>{},
              true,
              false,
              4U * 1024U * 1024U}) {}

platform::OperationResult ProductTranslationService::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::catalog
