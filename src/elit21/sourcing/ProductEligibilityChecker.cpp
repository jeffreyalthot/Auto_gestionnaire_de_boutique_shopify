#include "elit21/sourcing/ProductEligibilityChecker.h"

namespace elit21::sourcing {

ProductEligibilityChecker::ProductEligibilityChecker()
    : platform::BusinessComponent(
          "ProductEligibilityChecker",
          "autonomous supplier and product sourcing",
          platform::BusinessComponentSpec{
              "sourcing",
              "validate_transform",
              std::vector<std::string>{"product_id"},
              false,
              false,
              4U * 1024U * 1024U}) {}

platform::OperationResult ProductEligibilityChecker::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::sourcing
