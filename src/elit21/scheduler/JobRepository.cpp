#include "elit21/scheduler/JobRepository.h"

namespace elit21::scheduler {

JobRepository::JobRepository()
    : platform::BusinessComponent(
          "JobRepository",
          "scheduled and recurring job coordination",
          platform::BusinessComponentSpec{
              "scheduler",
              "model_operation",
              std::vector<std::string>{},
              false,
              false,
              4U * 1024U * 1024U}) {}

platform::OperationResult JobRepository::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::scheduler
