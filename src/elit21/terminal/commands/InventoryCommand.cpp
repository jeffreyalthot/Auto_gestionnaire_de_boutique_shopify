#include "elit21/terminal/commands/InventoryCommand.h"

namespace elit21::terminal::commands {

InventoryCommand::InventoryCommand()
    : platform::BusinessComponent(
          "InventoryCommand",
          "fixed-position terminal dashboard and command handling (commands)",
          platform::BusinessComponentSpec{
              "terminal",
              "model_operation",
              std::vector<std::string>{},
              false,
              false,
              4U * 1024U * 1024U}) {}

platform::OperationResult InventoryCommand::execute(const platform::OperationContext& context) {
    return executeBusiness(context);
}

} // namespace elit21::terminal::commands
