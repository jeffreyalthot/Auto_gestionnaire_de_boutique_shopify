#include "elit21/repositories/MetricRepository.h"

namespace elit21::repositories {

MetricRepository::MetricRepository()
    : platform::BusinessComponent(
          "MetricRepository",
          "persistent repository access",
          platform::BusinessComponentSpec{
              "repositories",
              "model_operation",
              std::vector<std::string>{},
              false,
              false,
              4U * 1024U * 1024U}) {}

platform::OperationResult MetricRepository::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::repositories
