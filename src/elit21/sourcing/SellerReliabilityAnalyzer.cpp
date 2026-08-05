#include "elit21/sourcing/SellerReliabilityAnalyzer.h"

namespace elit21::sourcing {

SellerReliabilityAnalyzer::SellerReliabilityAnalyzer()
    : platform::BusinessComponent(
          "SellerReliabilityAnalyzer",
          "autonomous supplier and product sourcing",
          platform::BusinessComponentSpec{
              "sourcing",
              "query",
              std::vector<std::string>{},
              false,
              false,
              4U * 1024U * 1024U}) {}

platform::OperationResult SellerReliabilityAnalyzer::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::sourcing
