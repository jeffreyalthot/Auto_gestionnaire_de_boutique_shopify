#include "elit21/automation/EmergencyKillSwitch.h"

namespace elit21::automation {

EmergencyKillSwitch::EmergencyKillSwitch()
    : platform::BusinessComponent(
          "EmergencyKillSwitch",
          "autonomous policy and task orchestration",
          platform::BusinessComponentSpec{
              "automation",
              "model_operation",
              std::vector<std::string>{},
              false,
              false,
              4U * 1024U * 1024U}) {}

platform::OperationResult EmergencyKillSwitch::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::automation
