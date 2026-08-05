#include "elit21/compliance/ProductRestrictionScanner.h"

namespace elit21::compliance {

ProductRestrictionScanner::ProductRestrictionScanner()
    : platform::BusinessComponent(
          "ProductRestrictionScanner",
          "Canadian product, privacy and address compliance",
          platform::BusinessComponentSpec{
              "compliance",
              "model_operation",
              std::vector<std::string>{},
              false,
              false,
              4U * 1024U * 1024U}) {}

platform::OperationResult ProductRestrictionScanner::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::compliance
