#include "elit21/terminal/commands/SourcingCommand.h"

namespace elit21::terminal::commands {

SourcingCommand::SourcingCommand()
    : platform::BusinessComponent(
          "SourcingCommand",
          "fixed-position terminal dashboard and command handling (commands)",
          platform::BusinessComponentSpec{
              "terminal",
              "model_operation",
              std::vector<std::string>{},
              false,
              false,
              4U * 1024U * 1024U}) {}

platform::OperationResult SourcingCommand::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::terminal::commands
