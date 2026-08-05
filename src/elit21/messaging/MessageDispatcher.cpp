#include "elit21/messaging/MessageDispatcher.h"

namespace elit21::messaging {

MessageDispatcher::MessageDispatcher()
    : platform::BusinessComponent(
          "MessageDispatcher",
          "command, query and event dispatch",
          platform::BusinessComponentSpec{
              "messaging",
              "command",
              std::vector<std::string>{},
              true,
              false,
              4U * 1024U * 1024U}) {}

platform::OperationResult MessageDispatcher::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::messaging
