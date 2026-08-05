#include "elit21/sourcing/CandidateProduct.h"

namespace elit21::sourcing {

CandidateProduct::CandidateProduct()
    : platform::BusinessComponent(
          "CandidateProduct",
          "autonomous supplier and product sourcing",
          platform::BusinessComponentSpec{
              "sourcing",
              "model_operation",
              std::vector<std::string>{},
              false,
              false,
              4U * 1024U * 1024U}) {}

platform::OperationResult CandidateProduct::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::sourcing
