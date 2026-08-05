#include "elit21/catalog/CollectionAssignmentEngine.h"

namespace elit21::catalog {

CollectionAssignmentEngine::CollectionAssignmentEngine()
    : platform::BusinessComponent(
          "CollectionAssignmentEngine",
          "catalog normalization and publication",
          platform::BusinessComponentSpec{
              "catalog",
              "orchestrate",
              std::vector<std::string>{},
              true,
              false,
              4U * 1024U * 1024U}) {}

platform::OperationResult CollectionAssignmentEngine::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::catalog
