#include "elit21/sourcing/SourcingDecision.h"

namespace elit21::sourcing {

SourcingDecision::SourcingDecision()
    : platform::BusinessComponent(
          "SourcingDecision",
          "autonomous supplier and product sourcing",
          platform::BusinessComponentSpec{
              "sourcing",
              "model_operation",
              std::vector<std::string>{},
              false,
              false,
              4U * 1024U * 1024U}) {}

platform::OperationResult SourcingDecision::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::sourcing
