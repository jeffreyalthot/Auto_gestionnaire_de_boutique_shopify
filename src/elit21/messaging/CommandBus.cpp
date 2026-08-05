#include "elit21/messaging/CommandBus.h"

namespace elit21::messaging {

CommandBus::CommandBus()
    : platform::BusinessComponent(
          "CommandBus",
          "command, query and event dispatch",
          platform::BusinessComponentSpec{
              "messaging",
              "model_operation",
              std::vector<std::string>{},
              false,
              false,
              4U * 1024U * 1024U}) {}

platform::OperationResult CommandBus::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::messaging
