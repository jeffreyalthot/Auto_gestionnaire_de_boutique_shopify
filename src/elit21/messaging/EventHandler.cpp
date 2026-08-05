#include "elit21/messaging/EventHandler.h"

namespace elit21::messaging {

EventHandler::EventHandler()
    : platform::BusinessComponent(
          "EventHandler",
          "command, query and event dispatch",
          platform::BusinessComponentSpec{
              "messaging",
              "model_operation",
              std::vector<std::string>{},
              false,
              false,
              4U * 1024U * 1024U}) {}

platform::OperationResult EventHandler::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::messaging
