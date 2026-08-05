#include "elit21/messaging/EventBus.h"

namespace elit21::messaging {

EventBus::EventBus()
    : platform::BusinessComponent(
          "EventBus",
          "command, query and event dispatch",
          platform::BusinessComponentSpec{
              "messaging",
              "model_operation",
              std::vector<std::string>{},
              false,
              false,
              4U * 1024U * 1024U}) {}

platform::OperationResult EventBus::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::messaging
