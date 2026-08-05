#include "elit21/automation/SimulationMode.h"

namespace elit21::automation {

SimulationMode::SimulationMode()
    : platform::BusinessComponent(
          "SimulationMode",
          "autonomous policy and task orchestration",
          platform::BusinessComponentSpec{
              "automation",
              "model_operation",
              std::vector<std::string>{},
              false,
              false,
              4U * 1024U * 1024U}) {}

platform::OperationResult SimulationMode::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::automation
