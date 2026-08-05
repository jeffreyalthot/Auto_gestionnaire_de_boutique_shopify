#include "elit21/automation/SelfTestManager.h"

namespace elit21::automation {

SelfTestManager::SelfTestManager()
    : platform::BusinessComponent(
          "SelfTestManager",
          "autonomous policy and task orchestration",
          platform::BusinessComponentSpec{
              "automation",
              "orchestrate",
              std::vector<std::string>{},
              true,
              false,
              4U * 1024U * 1024U}) {}

platform::OperationResult SelfTestManager::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::automation
