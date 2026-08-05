#include "elit21/repositories/PriceHistoryRepository.h"

namespace elit21::repositories {

PriceHistoryRepository::PriceHistoryRepository()
    : platform::BusinessComponent(
          "PriceHistoryRepository",
          "persistent repository access",
          platform::BusinessComponentSpec{
              "repositories",
              "model_operation",
              std::vector<std::string>{},
              false,
              false,
              4U * 1024U * 1024U}) {}

platform::OperationResult PriceHistoryRepository::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::repositories
