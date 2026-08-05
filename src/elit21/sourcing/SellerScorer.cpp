#include "elit21/sourcing/SellerScorer.h"

namespace elit21::sourcing {

SellerScorer::SellerScorer()
    : platform::BusinessComponent(
          "SellerScorer",
          "autonomous supplier and product sourcing",
          platform::BusinessComponentSpec{
              "sourcing",
              "validate_transform",
              std::vector<std::string>{},
              false,
              false,
              4U * 1024U * 1024U}) {}

platform::OperationResult SellerScorer::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::sourcing
