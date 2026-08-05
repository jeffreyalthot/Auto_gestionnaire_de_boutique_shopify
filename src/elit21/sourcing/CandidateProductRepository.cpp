#include "elit21/sourcing/CandidateProductRepository.h"

namespace elit21::sourcing {

CandidateProductRepository::CandidateProductRepository()
    : platform::BusinessComponent(
          "CandidateProductRepository",
          "autonomous supplier and product sourcing",
          platform::BusinessComponentSpec{
              "sourcing",
              "model_operation",
              std::vector<std::string>{},
              false,
              false,
              4U * 1024U * 1024U}) {}

platform::OperationResult CandidateProductRepository::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::sourcing
