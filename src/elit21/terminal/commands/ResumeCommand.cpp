#include "elit21/terminal/commands/ResumeCommand.h"

namespace elit21::terminal::commands {

ResumeCommand::ResumeCommand()
    : platform::BusinessComponent(
          "ResumeCommand",
          "fixed-position terminal dashboard and command handling (commands)",
          platform::BusinessComponentSpec{
              "terminal",
              "model_operation",
              std::vector<std::string>{},
              false,
              false,
              4U * 1024U * 1024U}) {}

platform::OperationResult ResumeCommand::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::terminal::commands
