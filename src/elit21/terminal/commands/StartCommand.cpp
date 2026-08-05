#include "elit21/terminal/commands/StartCommand.h"

namespace elit21::terminal::commands {

StartCommand::StartCommand()
    : platform::BusinessComponent(
          "StartCommand",
          "fixed-position terminal dashboard and command handling (commands)",
          platform::BusinessComponentSpec{
              "terminal",
              "model_operation",
              std::vector<std::string>{},
              false,
              false,
              4U * 1024U * 1024U}) {}

platform::OperationResult StartCommand::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::terminal::commands
