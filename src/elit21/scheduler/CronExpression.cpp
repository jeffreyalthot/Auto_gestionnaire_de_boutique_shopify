#include "elit21/scheduler/CronExpression.h"

namespace elit21::scheduler {

CronExpression::CronExpression()
    : platform::BusinessComponent(
          "CronExpression",
          "scheduled and recurring job coordination",
          platform::BusinessComponentSpec{
              "scheduler",
              "model_operation",
              std::vector<std::string>{},
              false,
              false,
              4U * 1024U * 1024U}) {}

platform::OperationResult CronExpression::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::scheduler
