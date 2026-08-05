#include "elit21/sourcing/ProductCandidatePipeline.h"

namespace elit21::sourcing {

ProductCandidatePipeline::ProductCandidatePipeline()
    : platform::BusinessComponent(
          "ProductCandidatePipeline",
          "autonomous supplier and product sourcing",
          platform::BusinessComponentSpec{
              "sourcing",
              "orchestrate",
              std::vector<std::string>{},
              true,
              false,
              4U * 1024U * 1024U}) {}

platform::OperationResult ProductCandidatePipeline::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::sourcing
