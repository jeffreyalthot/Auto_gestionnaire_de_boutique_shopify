#include "elit21/sourcing/ProhibitedProductDetector.h"

namespace elit21::sourcing {

ProhibitedProductDetector::ProhibitedProductDetector()
    : platform::BusinessComponent(
          "ProhibitedProductDetector",
          "autonomous supplier and product sourcing",
          platform::BusinessComponentSpec{
              "sourcing",
              "query",
              std::vector<std::string>{},
              false,
              false,
              4U * 1024U * 1024U}) {}

platform::OperationResult ProhibitedProductDetector::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::sourcing
