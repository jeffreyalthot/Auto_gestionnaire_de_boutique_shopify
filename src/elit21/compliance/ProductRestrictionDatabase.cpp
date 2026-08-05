#include "elit21/compliance/ProductRestrictionDatabase.h"

namespace elit21::compliance {

ProductRestrictionDatabase::ProductRestrictionDatabase()
    : platform::BusinessComponent(
          "ProductRestrictionDatabase",
          "Canadian product, privacy and address compliance",
          platform::BusinessComponentSpec{
              "compliance",
              "model_operation",
              std::vector<std::string>{},
              false,
              false,
              4U * 1024U * 1024U}) {}

platform::OperationResult ProductRestrictionDatabase::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::compliance
