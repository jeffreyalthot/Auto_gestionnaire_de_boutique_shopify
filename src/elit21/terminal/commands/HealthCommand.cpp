#include "elit21/terminal/commands/HealthCommand.h"

namespace elit21::terminal::commands {

HealthCommand::HealthCommand()
    : platform::BusinessComponent(
          "HealthCommand",
          "fixed-position terminal dashboard and command handling (commands)",
          platform::BusinessComponentSpec{
              "terminal",
              "query",
              std::vector<std::string>{},
              false,
              false,
              4U * 1024U * 1024U}) {}

platform::OperationResult HealthCommand::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::terminal::commands
