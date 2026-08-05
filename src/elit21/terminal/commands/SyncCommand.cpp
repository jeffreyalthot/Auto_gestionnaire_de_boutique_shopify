#include "elit21/terminal/commands/SyncCommand.h"

namespace elit21::terminal::commands {

SyncCommand::SyncCommand()
    : platform::BusinessComponent(
          "SyncCommand",
          "fixed-position terminal dashboard and command handling (commands)",
          platform::BusinessComponentSpec{
              "terminal",
              "command",
              std::vector<std::string>{},
              true,
              false,
              4U * 1024U * 1024U}) {}

platform::OperationResult SyncCommand::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::terminal::commands
