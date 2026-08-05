#include "elit21/repositories/TrackingRepository.h"

namespace elit21::repositories {

TrackingRepository::TrackingRepository()
    : platform::BusinessComponent(
          "TrackingRepository",
          "persistent repository access",
          platform::BusinessComponentSpec{
              "repositories",
              "model_operation",
              std::vector<std::string>{},
              false,
              false,
              4U * 1024U * 1024U}) {}

platform::OperationResult TrackingRepository::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::repositories
