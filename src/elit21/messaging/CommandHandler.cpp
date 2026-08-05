#include "elit21/messaging/CommandHandler.h"

namespace elit21::messaging {

CommandHandler::CommandHandler()
    : platform::BusinessComponent(
          "CommandHandler",
          "command, query and event dispatch",
          platform::BusinessComponentSpec{
              "messaging",
              "model_operation",
              std::vector<std::string>{},
              false,
              false,
              4U * 1024U * 1024U}) {}

platform::OperationResult CommandHandler::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::messaging
