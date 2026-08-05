#include "elit21/sourcing/ProductSelectionPolicy.h"

namespace elit21::sourcing {

ProductSelectionPolicy::ProductSelectionPolicy()
    : platform::BusinessComponent(
          "ProductSelectionPolicy",
          "autonomous supplier and product sourcing",
          platform::BusinessComponentSpec{
              "sourcing",
              "validate_transform",
              std::vector<std::string>{"product_id"},
              false,
              false,
              4U * 1024U * 1024U}) {}

platform::OperationResult ProductSelectionPolicy::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::sourcing
