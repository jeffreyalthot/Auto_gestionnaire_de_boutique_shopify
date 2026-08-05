#include "elit21/terminal/commands/BackupCommand.h"

namespace elit21::terminal::commands {

BackupCommand::BackupCommand()
    : platform::BusinessComponent(
          "BackupCommand",
          "fixed-position terminal dashboard and command handling (commands)",
          platform::BusinessComponentSpec{
              "terminal",
              "command",
              std::vector<std::string>{},
              true,
              false,
              4U * 1024U * 1024U}) {}

platform::OperationResult BackupCommand::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::terminal::commands
